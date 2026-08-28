#!/usr/bin/env bash

# XXX: (bbb) harness testrunning old uc inc loader

scr_pre=tool/local
\builtin . $scr_pre/common_script.bash
echo ${0##*/}: init done, mode is now $-

usp --reload uc-loader

uc_inc_pre "$@" # usrtools_usrconf/uc_loader
#. <(uc_inc_pre usrtools_usrconf/uc_loader)

#
