#!/bin/bash
#
# run-envd.sh
# Copyright (C) 2026 hari <hari@t470p>
#
# Distributed under terms of the MIT license.
#
DEBUG=1
VERBOSE=1
scr_pre=tool/local
\builtin . $scr_pre/common_script.bash
usp us-arr
\builtin . $scr_pre/common_libenv.bash
\builtin . $scr_pre/common-dsl.bash
echo ${0##*/}: init done, mode is now $-
usp us-envd
: env "${US_PP_RUN_SPEC:=us_pp_{input.cb:._set-source,params.A:{reader.cb,matcher.cb,emitter.cb\}\}}"
:spec-read() { User-Script.EnvD.read-spec "$@"; }
:to-v declare -p US_PP_RUN_SPEC
:spec-read US_PP_RUN_SPEC "$@"
:to-v declare -p us_pp
#
