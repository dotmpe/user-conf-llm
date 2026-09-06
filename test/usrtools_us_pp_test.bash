#!/usr/bin/env bash

# Initial test setup for bare source.
# Want some vanilla bash support in barebonebootstrap profile...

_us_pp_loads() {
  _inline_fun_tpl=$(:funbody :inline.fun)
  #shellcheck disable=2139  # var is expanded from tpl on assign
  alias inline-fun="${_inline_fun_tpl//_%_/___}"
}

function set_up() {
  pack_pre=usrtools_usrscr
  pack_src=src/$pack_pre/us_pp.inc
  pack_mod=Pre-Processor
  declare -gn hooks=user_script_cache__hooks
  echo "setting up" >&2
  shopt -s expand_aliases
  #shopt -s extdebug
  declare -gA _os_script_{load,path}
  _us_pp_loads
}

function test_us_pp_loads_inc_src() {
  . $pack_src
}

function test_us_pp_loads_inc_pre() {
  . $pack_src &&
  "$pack_mod" &&
  ._hooks:global &&
  ._hooks:load
}

function test_us_pp_loads_inc_main() {
  . $pack_src &&
  #. <(printf '%s\n' "${hooks[global]}" "${hooks[load]}") &&
  .run $pack_pre/us_pp.inc
}

# Id: us_pp_test                                 vim:set ft=bash sw=2 sts=2 et:
