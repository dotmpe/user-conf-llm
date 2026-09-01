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

\builtin . ./common_env.bash
METADIR=.local

\builtin . ./common-dsl.bash

#us_part $usp_opts --reload uc-loader
#. <(uc_inc_pre usrtools_usrconf/uc_docker)

DEBUG=1 VERBOSITY=4
. src/usrtools_usrscr/us_pp.inc
#.run src/usrtools_usrscr/us_pp.inc >/dev/null

mkdir -pv dist/usrtools_usrscr/
> dist/usrtools_usrscr/us_pp.bash \
  .run src/usrtools_usrscr/us_pp.inc

#. <(.run src/usrtools_usrscr/us_pp.inc)

#declare -f User-Script.Pre-Processor

#
