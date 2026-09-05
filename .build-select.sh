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

xredo_all_targets=( @build @test @pack )

case "${XREDO_TARGET}" in

( @build )
    sources=( src/*/*.inc )
    for src in "${sources[@]}"; do
      : "pack/ns1/${src#src/}"
      targets+=( "${_%.inc}.bash" )
    done
    redo-ifchange .build-select.sh "${targets[@]}"
  ;;

( @config )
    TODO config
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
    # TODO package
  ;;


( pack/*/* )
    : "${XREDO_TARGET#pack/ns[0-9]/}"
    src=src/${_%.bash}.inc
    redo-ifchange .build-select.sh &&
    redo-ifchange "$src" &&
    mkdir -p "${XREDO_TARGET%/*}" &&
    \builtin . ./init-pp.sh >&2 &&
    .run "$src" .match-line > "$BUILD_TARGET_TMP" ||
      failerr "Building ns1 for ${src@Q}"
  ;;


( * )
    return ${_E_next:-196}

esac
