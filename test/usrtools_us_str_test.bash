#!/usr/bin/env bash

function set_up() {
  pack_pre=usrtools_usrscr
  pack_src=src/$pack_pre/us_str.inc
  pack_mod=String
}

function test_us_str_loads_inc_src() {
:expect "Module loads, giving status 0"
  . ${pack_src:?}
}

function test_us_str_loads_inc_pre() {
:expect 'Main and hooks are runnable, give status 0'
  . $pack_src &&
  "$pack_mod"
}

function test_us_str_join_array() {
:expect 'concatenates items at delimiter'
  . $pack_src &&
  testSrcArr=( 1 2 3 ) &&
  .join-array test{Dest,SrcArr} , &&
  assert_same "${testDest?}" "1,2,3"
}

#function test_us_str_name_concat_names_right() {
## :expect ''
#  TODO
#}
#
#function test_us_str_output_slice() {
#:expect 'slices lines (streaming)'
#  TODO
#}

function test_us_str_wordmatch_any() {
:expect 'matches word/words'
  . $pack_src &&
  .wordmatch-any word abs word xyz &&
  .wordmatch-any word word abs xyz &&
  .wordmatch-any word abs xyz word
}

# Id: usrtools_str                                vim:set ft=bash sw=2 sts=2 et:
