#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
function set_up() {
  pack_pre=usrtools_usrconf
  pack_src=src/$pack_pre/uc_dev.inc
  declare -gn hooks=user_script_cache__hooks
}

function test_uc_dev_loads_inc_src() {
  . $pack_src
}

function test_uc_dev_loads_inc_pre() {
  . $pack_src &&
  . <(printf '%s\n' "${hooks[global]}" "${hooks[load]}")
}

# Id: usrtools_uc_dev_test                          vim:set ft=bash sw=2 sts=2 et:
