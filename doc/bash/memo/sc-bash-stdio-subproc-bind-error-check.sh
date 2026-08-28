#!/bin/bash
#
# sc-bash-stdio-subproc-bind-error-check.sh
# Copyright (C) 2026 hari <hari@t470p>
#
# Distributed under terms of the MIT license.
#
set -eETuo pipefail
shopt -s extdebug
IFS=$' \t\n'

exec {FD1}> <(echo)  # False: bind output to subprocess output?
exec {FD1}< >(echo)  # False: bind input to subprocess input?
echo $FD1
