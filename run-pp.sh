#!/bin/bash
#
# run-pp.sh
# Copyright (C) 2026 hari <hari@t470p>
#
# Distributed under terms of the MIT license.
#
scr_pre=tool/local
\builtin . $scr_pre/common_script.bash
\builtin . $scr_pre/common-dsl.bash

usp uc-cache

echo ${0##*/}: init done, mode is now $-

#us_part $usp_opts --reload uc-loader
#. <(uc_inc_pre usrtools_usrconf/uc_docker)

DEBUG=0 VERBOSITY=4
\builtin . ./init-pp.sh

#to-v type inline-fun
.run src/usrtools_usrscr/us_pp.inc .match-line
#>/dev/null

#mkdir -pv pack/ns0/usrtools_usrscr/
#> pack/ns0/usrtools_usrscr/us_pp.bash \
#  .run src/usrtools_usrscr/us_pp.inc us_fmt_inc

#. <(.run src/usrtools_usrscr/us_pp.inc)
#declare -f User-Script.Pre-Processor
#
