#!/bin/bash
#
# run-pp.sh
# Copyright (C) 2026 hari <hari@t470p>
#
# Distributed under terms of the MIT license.
#

\builtin . ./common_setup.bash
shopt -s extdebug expand_aliases

us-env -R us-env
declare -gA _os_script_{load,path}
us_part --hooks:declare,define,init us-term
: "${usp_opts:=--alias --hooks:declare,define,init --export}"
usp() { us_part $usp_opts "$@"; }

\builtin . ./common_env.bash
METADIR=.local

usp uc-cache

\builtin . ./common-dsl.bash

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
