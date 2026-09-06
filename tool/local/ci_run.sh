#! /bin/sh

ETC=.local/etc
VAR=.local/var
mkdir -vp "$ETC" "$VAR" >&2
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

redo -k @config all
