#!/usr/bin/env bash

# Initial test setup for bare source.
# Want some vanilla bash support in barebonebootstrap profile...
pack_pre=src/usrtools_usrscr

:inline.fun() {
  \builtin . <(sh_funbody ${_%_})
}

_us_pp_loads() {
  _inline_fun_tpl=$(sh_funbody :inline.fun)
  #shellcheck disable=2139  # var is expanded from tpl on assign
  alias inline-fun="${_inline_fun_tpl//_%_/___}"
}

function set_up() {
  echo "setting up" >&2
  shopt -s expand_aliases
  #shopt -s extdebug
  declare -gA _os_script_{load,path}
  _us_pp_loads
}

function test_us_pp_loads_inc_src() {
  . $pack_pre/us_pp.inc
}

function test_us_pp_loads_inc_pre() {
  . $pack_pre/us_pp.inc &&
  ._hooks:global &&
  ._hooks:load
}

function test_us_pp_loads_inc_run() {
  . $pack_pre/us_pp.inc &&
  ._hooks:global &&
  ._hooks:load &&
  : #.run $pack_pre/us_pp.inc
}

# Id: us_pp_test                                 vim:set ft=bash sw=2 sts=2 et:
