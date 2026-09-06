#!/usr/bin/env bash

# common_build.sh is the home for all target recipes

:uc-diag:forbidden-patterns() {
  # NOTE: this file and other tools are in tool/local/*, so patterns
  # themselves must (in general) be kept as config (elsewhere).
  redo-ifchange etc/diag_forbidden_patterns.bash.lines &&
  :pass "$(< $_ )" &&
  \builtin . <(printf "forbidden=(\m%s\n)" "$_") &&
  [[ ${forbidden[*]:+set} ]] ||
    :failerr "No forbidden patterns configured" || return

  # Search (grep) file for certain expressions, warn about match(es)
  for x in "${forbidden[@]}"; do
    :to-v grep -HPn "^[^#:]*$x" "$script" || continue
    :failerr "Found forbidden ${x@Q}, see before lines" || return
  done
}

:uc-diag:shell-lint-check() {
  ( \builtin . "$script" ) ||
    :failerr "E$? on test-loading ${script@Q}" || return
  \builtin command shellcheck "$script" >&2 &&
  say.v "Load and shellcheck passed for ${script@Q}"
}

:uc-diag:unguarded-tooling-invocations() {

  # FIXME: this does not work right yet; should require \builtin command for
  # certain toolkit commands (for recognition)
  # And for source/. (eval is kept in forbidden expressions)
  # But for other may introduce \reserved or \uc_reserved or similar. And should
  # know (scan/index) those with us-pp.
  # Same for some other commands, should require \inline prefix (later).
  redo-ifchange etc/diag_core_tooling.list &&
  mapfile -t cmds < $_ &&
  [[ ${cmds[*]:+set} ]] ||
    :failerr "No cmds configured" || return

  :pass "$(IFS='|'; echo "${cmds[*]}")" &&
  :to-v grep -HPn "^[^#:]*(?<!\\\bbuiltin[ \t])(?<!\\\)\b(${_:?})\b" -- "$script" ||
    return 0
  :failerr "Found unguarded tooling invocation(s), see before lines"
}

:uc-diag:todo-comments() {
  TODO "implement comment scan"
}

:xredo-check-recipe() {
  local diag script
  : "${XREDO_TARGET#@check:}"; IFS=: read -r script diag <<<"${_}" &&
  : "${script:?$(:unset-err script 'Input source file')}"

  redo-ifchange "$script" &&
  # TODO: act on and handle $diag setting
  case "$script" in

  ( pack/* )
      # TODO: rewrite parts so they can be used as recipe target
      #: "${diag:=@uc-diag:shell-lint-check}"
      :uc-diag:shell-lint-check
    ;;

  ( *.do | tool/local/common* )
      :uc-diag:shell-lint-check &&
      :uc-diag:forbidden-patterns &&
      #:uc-diag:unguarded-tooling-invocations &&
      : # :uc-diag:todo-comments
    ;;

  ( src/* | test/* | tool/* )
      :uc-diag:forbidden-patterns &&
      : #:uc-diag:todo-comments
    ;;

  ( * ) :failerr "There is no check action for script ${script@Q}"
  esac
}

:xredo-check-target() {
  redo-always
  local files targets
  files=(
    default.do
    src/*/*.inc
    test/*.*
    tool/bash/part/*
    tool/local/{,exec/}*.*
  )
  for file in "${files[@]}"; do
    # TODO: make some grouping(s) of diag/src sets, not all should always need
    # to be on. CI would have the most complete set, then the (full) test
    # branch, but other envs/branches may get fewer diag (or none; ie "dev")
    # @uc-diag:regression-grep
    targets+=( "@check:$file" )
  done
  redo-ifchange "${targets[@]}"
}

:xredo-build-target() {
  \builtin . ./$VAR/redo_default.bash &&
  for src in "${sources[@]:?}"; do
    src=${src#src/}
    targets+=( "@index:${src:?}" )
    targets+=( "pack/ns1/${src%.inc}.bash" )
  done &&
  #:to-v :dump-pretty-globals sources targets &&
  redo-ifchange ${scr_pre:?}/build-select.sh "${sources[@]}" "${targets[@]}"
}

:xredo-config-target() {
  redo-ifchange ${scr_pre:?}/build-select.sh
  sources=( src/*/*.inc ) &&
  # TODO: settle on failglob or not
  [[ ${sources[*]:+set} ]] || say.err "No sources found" || exit

  tools=( tool/local/exec/*.* ) &&
  for tool in "${tools[@]}"; do
    [[ -x "$tool" ]] || continue
    scr=${tool##*/}
    if [[ -h $scr && ! -e $scr ]]; then rm "$scr"; fi
    if [[ ! -h $scr ]]; then
      if [[ -e $scr ]]; then
        say.err "config: Local tool path exists: ${scr@Q} (ignored)"
        continue
      fi
      :to-v ln -sv "$tool" ${tool##*/} || return
    fi
  done

  #:dump-globals sources >| ./$VAR/redo_default.bash &&
  :dump-pretty-globals sources >| ./$VAR/redo_default.bash &&
  redo-stamp <<< "${sources[@]}"
}

:xredo-index-recipe() {
  src=src/${XREDO_TARGET#@index:}
  redo-ifchange "$src" &&
  \builtin . ${scr_pre:?}/init-pp.sh >&2 &&
  .run "$src" .match-line > /dev/null || :failerr "Indexing ${src@Q}"
}

:xredo-pack-recipe() {
  : "${XREDO_TARGET#pack/ns[0-9]/}"
  src=src/${_%.bash}.inc
  redo-ifchange ${scr_pre:?}/build-select.sh "$src" &&
  mkdir -p "${XREDO_TARGET%/*}" &&
  \builtin . ${scr_pre:?}/init-pp.sh >&2 &&
  .run "$src" .match-line > "$BUILD_TARGET_TMP" ||
    :failerr "Building ns1 for ${src@Q}"
}

:xredo-pack-target() {
  redo-always
  TODO package
}

:xredo-test-recipe() {
  modid=${XREDO_TARGET#@test:}
  if ! (shopt -s failglob; : test/"${modid:?}"_test.* ) 2>/dev/null; then
    say.err "No tests for $modid"
    # TODO: require tests later
    return
  fi

  :cache-load ./etc/bash/us_bbb_specials.bash &&
  export -f "${us_bbb_specials[@]:?}" ||
    say.err "Failed at loading specials" || return

  #shellcheck disable=2295
  tests=( test/"${modid:?}"_test.* ) &&
  redo-ifchange @test:config "${tests[@]}" &&
  testid=$(sha256sum < <(printf '%s\n' "${tests[@]}")) &&
  : $'[\t ]' &&
  testid=${testid%%$_*} &&
  # >&2 declare -p testid &&
  mkdir -p .local/build &&
  \builtin command bashunit \
    --env test/_test_bootstrap.sh \
    --log-junit .local/build/test-report-$testid.xml \
    --coverage --coverage-min 80 \
    "${tests[@]}" >&2
}

:xredo-test-target() {
  redo-always
  say.debug "Starting pre-test checks"
  for x in pack/ns1/usrtools_usr{conf,scr}/*.bash; do
    targets+=( @check:"$x" )
  done
  redo-ifchange "${targets[@]}" || return
  unset targets

  say.info "All current packs checked OK, starting tests..."
  for x in pack/ns1/usrtools_usr{conf,scr}/*.bash; do
    : "${x##pack/ns1/}"
    : "${_%.bash}"
    : "${_/usrconf\/}"
    : "${_/usrscr\/}"
    targets+=( @test:"$_" )
  done
  redo-ifchange "${targets[@]}" || return
}

# Id: common_build                               vim:set ft=bash sw=2 sts=2 et:
