#!/bin/bash
#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
set -euo pipefail
shopt -s failglob nullglob
IFS=$' \t\n'

default_do_env() {
  \builtin . ./common_env.bash
  \builtin . ./common-dsl.bash
}

default_do_main() {
  declare BUILD_TARGET=${1:?}
  declare BUILD_TARGET_BASE=$2
  declare BUILD_TARGET_TMP=$3

  default_do_env ||
    failerr "E$? $_" || return

  declare -I BUILD_SELECT_SH
  if [[ ! -e "${BUILD_SELECT_SH:=./.build-select.sh}" ]]
  then unset BUILD_SELECT_SH
  else
    \builtin . "${BUILD_SELECT_SH:?}" && exit || {
      local st=$?
      (( st == _E_next )) || exit $st
    }
  fi

  case "${1:?}" in

  ( "${HELP_TARGET:-help}"|-help|-h )
        ${BUILD_TOOL:?}-always &&
        TODO
      ;;

    # Default build target
  ( all|@all|:all )
        TODO
      ;;

  ( * ) false
      ;;

  esac

  # End build if handler has not exit already
  exit $?
}

[[ ! ${REDO_RUNID:+set} ]] || {

  : "${US_DEBUG:=${DEBUG:=0}}"

  default_do_main "$@"
}

# Id: default         vim:set ft=bash sw=2 sts=2 et:
