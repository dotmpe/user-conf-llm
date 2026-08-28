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
  pack_mod=$(head -n 1 "$pack_src" | tr -d '() {')

  fmt_pack_src=src/$pack_pre/us_fmt_inc.inc
  fmt_pack_ns1=pack/ns1/$pack_pre/us_fmt_inc.bash
  ns_pack_ns1=pack/ns1/$pack_pre/us_ns.bash
  _us_pp_loads
}

function test_us_pp_loads_inc_src() {
:expect 'Module loads, giving status 0'
  . $pack_src
}

function test_us_pp_loads_inc_pre() {
:expect 'Main and hooks are runnable, give status 0'
  . $fmt_pack_ns1 &&
  . $ns_pack_ns1 &&
  . $pack_src &&
  "$pack_mod" &&
  ._hooks:global &&
  ._hooks:load
}

function test_us_pp_loads_inc_main() {
:expect 'Module can pre-process itself'
  . $pack_src &&
  #. <(printf '%s\n' "${hooks[global]}" "${hooks[load]}") &&
  .run src/$pack_pre/us_pp.inc >/dev/null
}

# Id: us_pp_test                                 vim:set ft=bash sw=2 sts=2 et:
