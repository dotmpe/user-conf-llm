#!/usr/bin/env bash

# common_build.sh is the home for all target recipes

:xredo-check-recipe() {
  script=${XREDO_TARGET#@check:}
  redo-ifchange "$script"
  case "$script" in
  ( pack/* )
      ( \builtin . "$script" ) || :failerr "Loading ${script@Q}" || return
      shellcheck "$script" >&2 &&
      say.v "Load and shellcheck passed for ${script@Q}"
    ;;
  ( * ) :failerr "There is no check action for script ${script@Q}"
  esac
}

:xredo-build-target() {
  \builtin . ./$VAR/redo_default.bash &&
  for src in "${sources[@]:?}"; do
    src=${src#src/}
    targets+=( "@index:${src:?}" )
    targets+=( "pack/ns1/${src%.inc}.bash" )
  done &&
  #>&2 :dump-pretty-globals sources targets &&
  redo-ifchange ${scr_pre:?}/build-select.sh "${sources[@]}" "${targets[@]}"
}

:xredo-config-target() {
  redo-ifchange ${scr_pre:?}/build-select.sh
  sources=( src/*/*.inc ) &&
  # TODO: settle on failglob or not
  [[ ${sources[*]:+set} ]] || say.err "No sources found" || exit

  tools=( tool/local/exec/*.* ) &&
  for tool in "${tools[@]}"; do
    scr=${tool##*/}
    if [[ -h $scr && ! -e $scr ]]; then rm "$scr"; fi
    if [[ ! -h $scr ]]; then
      if [[ -e $scr ]]; then
        say.err "config: Local tool path exists: ${scr@Q} (ignored)"
        continue
      fi
      >&2 ln -sv "$tool" ${tool##*/} || return
    fi
  done

  #:dump-global-pretty sources >| ./$VAR/redo_default.bash &&
  declare -p sources >| ./$VAR/redo_default.bash &&
  redo-stamp <<< "${sources[@]}"
}

:xredo-diag-target() {
  redo-always
  local tools targets
  tools=( tool/local/{,exec/}*.* )
  for tool in "${tools[@]}"; do
    targets+=( "@check:uc_diag_regression_grep:$tool" )
  done
  redo-ifchange "${targets[@]}"
}

:xredo-index-recipe() {
  src=src/${XREDO_TARGET#@index:}
  redo-ifchange "$src" &&
  \builtin . ${scr_pre:?}/init-pp.sh >&2 &&
  .run "$src" .match-line > /dev/null || failerr "Indexing ${src@Q}"
}

:xredo-pack-recipe() {
  : "${XREDO_TARGET#pack/ns[0-9]/}"
  src=src/${_%.bash}.inc
  redo-ifchange ${scr_pre:?}/build-select.sh "$src" &&
  mkdir -p "${XREDO_TARGET%/*}" &&
  \builtin . ${scr_pre:?}/init-pp.sh >&2 &&
  .run "$src" .match-line > "$BUILD_TARGET_TMP" ||
    failerr "Building ns1 for ${src@Q}"
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
  tests=( test/"${modid:?}"_test.* ) &&
  redo-ifchange @test:config "${tests[@]}" &&
  \builtin command bashunit --bootstrap test/_test_bootstrap.sh "${tests[@]}" >&2
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

#
