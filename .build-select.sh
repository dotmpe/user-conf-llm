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

xredo_all_targets=( @build @test )

case "${XREDO_TARGET}" in

( @build )
    redo-ifchange .build-select.sh \
      dist/usrtools_usrconf/uc_dev.bash \
      dist/usrtools_usrconf/uc_docker.bash \
      dist/usrtools_usrconf/uc_loader.bash \
      dist/usrtools_usrconf/uc_loader_meta.bash \
      dist/usrtools_usrconf/uc_profile.bash \
      dist/usrtools_usrscr/us_core.bash \
      dist/usrtools_usrscr/us_log.bash \
      dist/usrtools_usrscr/us_part.bash \
      dist/usrtools_usrscr/us_say.bash \
      dist/usrtools_usrscr/us_str.bash \
      dist/usrtools_usrscr/us_pp.bash
  ;;

( @config )
    TODO config
  ;;

( @test:* )
    local script=${XREDO_TARGET#@test:}
    redo-ifchange "$script"
    ( \builtin . "$script" ) ||
      failerr "Loading ${script@Q}" || return
    shellcheck "$script" >&2 &&
    say.v "Load and shellcheck passed for ${script@Q}"
  ;;

( @test )
    redo-always
    say.debug "Starting test"
    for x in dist/usrtools_usr{conf,scr}/*.bash; do
      targets+=( @test:"$x" )
    done
    redo-ifchange "${targets[@]}" || return
    say.info "All dist tested loaded OK"
  ;;

( dist/* )
    : "${XREDO_TARGET#dist/}"
    src=src/${_%.bash}.inc
    redo-ifchange .build-select.sh &&
    redo-ifchange "$src" &&
    mkdir -p "${XREDO_TARGET%/*}" &&
    \builtin . src/usrtools_usrscr/us_pp.inc &&
    cache_loadmaps "${US_PP_STATE:?}" us_pp_{name_map,meta_static} &&
    .run "$src" > "$BUILD_TARGET_TMP"
  ;;

( * )
    return ${_E_next:-196}

esac
