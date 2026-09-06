#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
function set_up() {
  pack_pre=usrtools_usrscr
  pack_src=src/$pack_pre/us_core.inc
  pack_mod=Core
  declare -gn hooks=user_script_cache__hooks
}

function test_us_core_loads_inc_src() {
  . $pack_src
}

function test_us_core_loads_inc_pre() {
  . $pack_src &&
  "$pack_mod" &&
  ._hooks:global &&
  ._hooks:load
}

# Id: usrtools_us_core_test                          vim:set ft=bash sw=2 sts=2 et:
