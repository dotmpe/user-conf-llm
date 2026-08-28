#!/bin/bash
#
# readloop-filter-example.sh
#
# Process substitution can be used with standard input or output binding for
# special reader or writer pipelines. Like any substitution they get full acess
# to *a copy* snapshot of the environment. They also inherit all current shell
# flags and options.
#
# Copyright (C) 2026 hari <hari@t470p>
#
# Distributed under terms of the MIT license.
#
set -euo pipefail
shopt -s extdebug
IFS=$' \t\n'

CTX='my context'
SPEC='$CTX'

filter() {
  while IFS= read -r line; do
    . <(echo "echo ${SPEC}: ${line@Q}")
  done
}

say() { printf '%s\n' "$*" >&$FD; }
cleanup() {
  exec {FD}>&-
  wait ${PID} 2>/dev/null || true
}
exec {FD}> >(filter) # Copy and start filter "shell" using process substitution
PID=$!
trap 'cleanup' EXIT
echo Running filter at $PID
say Foo
CTX='new context' # False: cannot not change CTX or SPEC in substitution
say Bar
# Simple in-based assigment of <CTX>=<new value> is the most direct way, and
# file IPC is the only way to communicate state change. But this is just a
# simple example of syntax and sound setup.
