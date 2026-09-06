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
scr_pre=tool/local

case "${XREDO_TARGET}" in @config | @*:config ) ;; ( * )
  if ! redo-ifdone @config; then
      say.err "Must run redo @config first"
      exit 1
  fi
esac

\builtin . $scr_pre/common_build.sh

case "${XREDO_TARGET}" in

( @build )
    :xredo-build-target &&
    redo-stamp <<< "$(sh_funbody $_)" &&
    redo-ifchange @build:config
  ;;

( @build:config )
    redo-ifchange $scr_pre/common_build.sh &&
    redo-stamp < <(grep -Po '^:xredo-[A-Za-z0-9-]+(?=\(\))' $scr_pre/common_build.sh)
  ;;

( @test:config )
    redo-ifchange test/_test_bootstrap.sh &&
    redo-stamp < <(grep -Pv '^([\t ]*|[\t ]*\#.*)$' test/_test_bootstrap.sh)
  ;;

( @config )
    :xredo-config-target &&
    redo-stamp <<< "$(sh_funbody $_)" &&
    redo-ifchange @build:config
  ;;

( @index:* )
    :xredo-index-recipe &&
    redo-stamp <<< "$(sh_funbody $_)" &&
    redo-ifchange @build:config
  ;;

( @test )
    :xredo-test-target &&
    redo-stamp <<< "$(sh_funbody $_)" &&
    redo-ifchange @build:config
  ;;

( @test:* )
    :xredo-test-recipe &&
    redo-stamp <<< "$(sh_funbody $_)" &&
    redo-ifchange @build:config
  ;;

( @check:* )
    :xredo-check-recipe &&
    redo-stamp <<< "$(sh_funbody $_)" &&
    redo-ifchange @build:config
  ;;

( @pack )
    :xredo-pack-target &&
    redo-stamp <<< "$(sh_funbody $_)" &&
    redo-ifchange @build:config
  ;;

( pack/*/* )
    :xredo-pack-recipe &&
    redo-stamp <<< "$(sh_funbody $_)" &&
    redo-ifchange @build:config
  ;;


( * )
    [[ -e ./$ETC/redo_default.bash ]] || {
      echo "xredo_all_targets=( @config @build @test )" > ./$ETC/redo_default.bash
    }
    cache_load ./$ETC/redo_default.bash

    return ${_E_next:-196}

esac
