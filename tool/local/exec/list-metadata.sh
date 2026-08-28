#!/bin/bash
#
# list-metadata.sh (x)
#
scr_pre=tool/local
\builtin . $scr_pre/common_script.bash
\builtin . $scr_pre/common-dsl.bash

:cache-loadmaps "${US_PP_STATE:?}" us_pp_{name_map,meta_static}

case "${1:-srcmap}" in
( srcmap ) printf.lines.array-map.tab us_pp_name_map ;;
( meta-static ) printf.lines.array-map.tab us_pp_meta_static ;;
( * )
    : "${1:?$(:argv-err 1 'Entry point')}"
    : "${_@Q}? unknown entry point"
    :failerr "$_, ${0##*/}"
esac
