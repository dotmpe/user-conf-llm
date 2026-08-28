#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.

function set_up() {
  pack_pre=usrtools_usrscr
  pack_src=src/$pack_pre/us_part.inc
  declare -gn hooks=user_script_cache__hooks
}

function test_us_part_loads_inc_src() {
  . $pack_src
}

function test_us_part_loads_inc_pre() {
  . $pack_src &&
  . <(printf '%s\n' "${hooks[global]}" "${hooks[load]}")
}

function test_us_part_loads_inc_main() {
  . $pack_src &&
  . <(printf '%s\n' "${hooks[global]}" "${hooks[load]}") &&
  .main $pack_pre/us_part.inc
}

# Id: usrtools_us_part_test                          vim:set ft=bash sw=2 sts=2 et:
