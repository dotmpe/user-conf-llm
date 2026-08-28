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
:expect "Module loads, giving status 0"
  . $pack_src
}

function test_uc_cache_loads_inc_pre() {
:expect 'Main and hooks are runnable, give status 0'
  . $pack_src &&
  ._hooks:global &&
  ._hooks:load
}

function test_uc_cache_loads_inc_post() {
:expect 'Package loads, gives status 0'
  . $pack_ns1 &&
  . <(printf '%s\n' "${hooks[global]}" "${hooks[load]}")
}

function test_uc_cache_define_object() {
  . $pack_ns1 &&
  . <(printf '%s\n' "${hooks[global]}" "${hooks[load]}") &&

  User-Conf.Cache.define-object
}

function test_uc_cache_delete_object() {
  . $pack_ns1 &&
  . <(printf '%s\n' "${hooks[global]}" "${hooks[load]}") &&

  User-Conf.Cache.define-object.delete
}

# Id: usrtools_uc_cache_test                          vim:set ft=bash sw=2 sts=2 et:
