#!/usr/bin/env bash
set -euo pipefail
IFS=$' \t\n'

:inline.fun() {
  \builtin . <(:funbody ${_%_})
}
