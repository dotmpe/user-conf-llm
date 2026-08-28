#!/usr/bin/env bash
set -euo pipefail
shopt -s extdebug
IFS=$' \t\n'

# TODO: want to have several base envs for testing; bc on dev env everything
# can be inherited that is where test env starts as well
if [[ ${US_BBB_ENV:+set} ]]; then
  : # XXX: dynamic reinit (arrays etc.)?
  #. <(printf '%s\n' "$US_BBB_ENV")
elif [[ ${US_ENV_INIT:+set} ]]; then
  declare -gA _os_script_{load,path}
  METADIR=.local
  us-env -R us-env
  us_part --hooks:define,declare,init us-bbb
  # uc-cache
fi

# :inline.fun() {
#   \builtin . <(:funbody ${_%_})
# }

# Test name space util

declare -gA pack_name2=(
  [us]=usrscr
  [uc]=usrconf
  [ut]=usrtools
)

:setup-for() {
  : "${1#test/}"
  test_pack_name="${_%_test.bash}"
  pack_name1_ns1=${test_pack_name%%_*}
  pack_mod_ns1=${test_pack_name#*_}
  pack_pre_ns1=${pack_name1_ns1}_${pack_name2["${pack_mod_ns1%%_*}"]}
  pack_src=src/${pack_pre_ns1}/${pack_mod_ns1}.inc
  pack_path_ns1=pack/ns1/$pack_pre_ns1/$pack_mod_ns1.bash

  #pack_mod_name_ns0=
  : "${pack_mod:=$(head -n 1 "$pack_src" | tr -d '() {')}"
}

:expect() {
  : "${FUNCNAME[1]}"
  : "${_#test_}"
  : "${_//_/ }"
  bashunit::set_test_title "${_@Q} $1"
}
