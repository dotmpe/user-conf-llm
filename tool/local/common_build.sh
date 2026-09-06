#!/usr/bin/env bash

# common_build.sh is the home for all target recipes

:xredo-check-recipe() {
  script=${XREDO_TARGET#@check:}
  redo-ifchange "$script"
  ( \builtin . "$script" ) || failerr "Loading ${script@Q}" || return
  shellcheck "$script" >&2 &&
  say.v "Load and shellcheck passed for ${script@Q}"
}

:xredo-build-target() {
  \builtin . ./$VAR/redo_default.bash &&
  for src in "${sources[@]:?}"; do
    src=${src#src/}
    targets+=( "@index:${src:?}" )
    targets+=( "pack/ns1/${src%.inc}.bash" )
  done &&
  #>&2 :dump-pretty-globals sources targets &&
  redo-ifchange .build-select.sh "${sources[@]}" "${targets[@]}"
}

:xredo-config-target() {
  redo-ifchange .build-select.sh
  sources=( src/*/*.inc ) &&
  [[ ${sources[*]:+set} ]] || say.err "No sources found" || exit
  #:dump-global-pretty sources >| ./$VAR/redo_default.bash &&
  declare -p sources >| ./$VAR/redo_default.bash &&
  redo-stamp <<< "${sources[@]}"
}

:xredo-index-recipe() {
  src=src/${XREDO_TARGET#@index:}
  redo-ifchange "$src" &&
  \builtin . ./init-pp.sh >&2 &&
  .run "$src" .match-line > /dev/null || failerr "Indexing ${src@Q}"
}

:xredo-pack-recipe() {
  : "${XREDO_TARGET#pack/ns[0-9]/}"
  src=src/${_%.bash}.inc
  redo-ifchange .build-select.sh "$src" &&
  mkdir -p "${XREDO_TARGET%/*}" &&
  \builtin . ./init-pp.sh >&2 &&
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
