#!/bin/bash
#
# .build-select.sh
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
#

XREDO_TARGET="${REDO_PWD:+$REDO_PWD/}${REDO_TARGET:?}"
# XRedo/Base: Actual initial (path, name or id) spec for target
XREDO_BASE=${XREDO_TARGET%%:*}
XREDO_NODE=${XREDO_TARGET%:*}

ETC=.local/etc
VAR=.local/var

case "${XREDO_TARGET}" in @config ) ;; ( * )
  if ! redo-ifdone @config; then
      say.err "Must run redo @config first"
      exit 1
  fi
esac

case "${XREDO_TARGET}" in

( @build )
    \builtin . ./$VAR/redo_default.bash &&
    for src in "${sources[@]:?}"; do
      src=${src#src/}
      targets+=( "@index:${src:?}" )
      targets+=( "pack/ns1/${src%.inc}.bash" )
    done &&
    #>&2 :dump-pretty-globals sources targets &&
    redo-ifchange .build-select.sh "${sources[@]}" "${targets[@]}"
  ;;

( @config )
    redo-ifchange .build-select.sh
    sources=( src/*/*.inc ) &&
    [[ ${sources[*]:+set} ]] || say.err "No sources found" || exit
    #:dump-global-pretty sources >| ./$VAR/redo_default.bash &&
    declare -p sources >| ./$VAR/redo_default.bash &&
    redo-stamp <<< "${sources[@]}"
  ;;

( @index:* )
    src=src/${XREDO_TARGET#@index:}
    redo-ifchange "$src" &&
    \builtin . ./init-pp.sh >&2 &&
    .run "$src" .match-line > /dev/null || failerr "Indexing ${src@Q}"
  ;;

( @test )
    redo-always
    say.debug "Starting test"
    for x in pack/ns1/usrtools_usr{conf,scr}/*.bash; do
      targets+=( @test:"$x" )
    done
    redo-ifchange "${targets[@]}" || return
    say.info "All packs tested loaded OK"
  ;;

( @test:* )
    script=${XREDO_TARGET#@test:}
    redo-ifchange "$script"
    ( \builtin . "$script" ) || failerr "Loading ${script@Q}" || return
    shellcheck "$script" >&2 &&
    say.v "Load and shellcheck passed for ${script@Q}"
  ;;

( @pack )
    redo-always
    TODO package
  ;;


( pack/*/* )
    : "${XREDO_TARGET#pack/ns[0-9]/}"
    src=src/${_%.bash}.inc
    redo-ifchange .build-select.sh "$src" &&
    mkdir -p "${XREDO_TARGET%/*}" &&
    \builtin . ./init-pp.sh >&2 &&
    .run "$src" .match-line > "$BUILD_TARGET_TMP" ||
      failerr "Building ns1 for ${src@Q}"
  ;;


( * )
    cache_load ./$ETC/redo_default.bash

    return ${_E_next:-196}

esac
