#!/usr/bin/env bash

# Initial test setup for bare source.
# Want some vanilla bash support in barebonebootstrap profile...

function set_up() {
  pack_mod=Namespace
  :setup-for ${BASH_SOURCE[0]}
  cache_pack_src=src/usrtools_usrconf/uc_cache.inc
  cache_pack_ns1=pack/ns1/usrtools_usrconf/uc_cache.bash
  declare -gn hooks=user_script_namespace__hooks
}

function test_us_ns_loads_inc_src() {
:expect "Module loads, giving status 0"
  . $pack_src
}

function test_us_ns_loads_inc_pre() {
:expect 'Main and hooks are runnable, give status 0'
  . $cache_pack_src &&
  . $pack_src &&
  ._hooks:global &&
  ._hooks:load
}

function test_us_ns_loads_inc_post() {
:expect 'Package loads, gives status 0'
  . $cache_pack_ns1 &&
  . $pack_path_ns1 &&
  . <(printf '%s\n' "${hooks[global]}" "${hooks[load]}")
}

# Id: usrtools_ns                                vim:set ft=bash sw=2 sts=2 et:
