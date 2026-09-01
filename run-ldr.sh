#!/usr/bin/env bash
#
# run-loader.sh
# Copyright (C) 2026 hari <hari@t470p>
#
# Distributed under terms of the MIT license.
#
\builtin . ./common_setup.bash

us-env -R us-env

declare -gA _os_script_{load,path}
us_part --hooks:declare,define,init us-term
: "${usp_opts:=--alias --hooks:declare,define,init --export}"

\builtin . ./common_env.bash
METADIR=.local

echo init done, mode is now $-

us_part $usp_opts --reload uc-loader
uc_inc_pre "$@" # usrtools_usrconf/uc_loader
#. <(uc_inc_pre usrtools_usrconf/uc_loader)

#
