#!/usr/bin/env bash

# Initial test setup for bare source.
# Want some vanilla bash support in barebonebootstrap profile...

function set_up() {
  :setup-for ${BASH_SOURCE[0]}
  #pack_pre=usrtools_usrscr
  #pack_src=src/$pack_pre/us_fmt_inc.inc
  #declare -gn hooks=user_script_format_include__hooks
}

function test_us_fmt_inc_loads_inc_src() {
:expect "Module loads, giving status 0"
  . $pack_src
}

function test_us_fmt_inc_loads_inc_pre() {
:expect 'Main and hooks are runnable, give status 0'
  . $pack_src &&
  "$pack_mod"
}

# Id: usrtools_us_fmt_inc                        vim:set ft=bash sw=2 sts=2 et:
