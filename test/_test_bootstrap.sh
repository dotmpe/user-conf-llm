#!/usr/bin/env bash
set -euo pipefail
IFS=$' \t\n'

:inline.fun() {
  \builtin . <(sh_funbody ${_%_})
}
