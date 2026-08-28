#!/bin/bash
#
# list-metadata.sh (x)
#
scr_pre=tool/local
\builtin . $scr_pre/common_script.bash
\builtin . $scr_pre/common-dsl.bash

:cache-loadmaps "${US_PP_STATE:?}" us_pp_{source_map,meta_block}

case "${1:-srcmap}" in
( srcmap ) :printf-arrtab us_pp_source_map ;;
( meta-static ) :printf-arrtab us_pp_meta_block ;;
( * )
    : "${1:?$(:argv-err 1 'Entry point')}"
    : "${_@Q}? unknown entry point"
    :failerr "$_, ${0##*/}"
esac
