#! /bin/sh

ETC=.local/etc
VAR=.local/var
mkdir -vp "$ETC" "$VAR" .local/user/data >&2
etc=./$ETC/redo_default.bash
var=./$VAR/redo_default.bash
[ -e "$var" ] || {
  touch "$var"
  echo "New file $var" >&2
}
[ -e $etc ] || {
  echo "xredo_all_targets=( @config @build @test @check )" > "$etc"
  echo "New file $etc" >&2
}

# Reset caches
rm -f .local/user/data/*

redo @config
redo -j10 -k @config all

# Id: ci_run                                       vim:set ft=sh sw=2 sts=2 et:
