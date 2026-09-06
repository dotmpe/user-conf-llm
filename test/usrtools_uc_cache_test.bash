#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
function set_up() {
  pack_pre=usrtools_usrconf
  pack_src=src/$pack_pre/uc_cache.inc
  pack_ns1=pack/ns1/$pack_pre/uc_cache.bash
  declare -gn hooks=user_script_cache__hooks
}

function test_uc_cache_loads_inc_src() {
  . $pack_src
}

function test_uc_cache_loads_inc_pre() {
  . $pack_src &&
  ._hooks:global &&
  ._hooks:load
}

function test_uc_cache_loads_inc_post() {
  . $pack_ns1 &&
  . <(printf '%s\n' "${hooks[global]}" "${hooks[load]}")
}

# Id: usrtools_uc_cache_test                          vim:set ft=bash sw=2 sts=2 et:
