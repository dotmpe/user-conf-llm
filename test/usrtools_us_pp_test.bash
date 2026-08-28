#!/usr/bin/env bash

# Initial test setup for bare source.
# Want some vanilla bash support in barebonebootstrap profile...

_us_pp_loads() {
  #shellcheck disable=2139  # var is expanded from tpl on assign
  _inline_fun_tpl=$(:funbody :inline.fun) &&
  alias inline-fun="${_inline_fun_tpl//_%_/___}"
}

function set_up() {
  :setup-for ${BASH_SOURCE[0]}
}

function test_us_pp_loads_inc_src() {
:expect 'Module loads, giving status 0'
  . ${pack_src:?}
}

function test_us_pp_loads_inc_pre() {
:expect 'Main and hooks are runnable, give status 0'

  bbb_load env_common dsl-common &&

  . $pack_src &&
  ._hooks:global &&
  ._hooks:load
}

function test_us_pp_loads_inc_main() {
:expect 'Module can pre-process itself'

  bbb_load env_common dsl-common &&

  . $pack_src &&
  ._hooks:global &&
  ._hooks:load &&

  #. <(printf '%s\n' "${hooks[global]}" "${hooks[load]}") &&
  .run src/${pack_pre:?}/us_pp.inc >/dev/null
}

# Id: us_pp_test                                 vim:set ft=bash sw=2 sts=2 et:
