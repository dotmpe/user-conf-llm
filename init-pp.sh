#!/bin/bash
#
# init-pp.sh
# Copyright (C) 2026 hari <hari@t470p>
#
# Distributed under terms of the MIT license.
#

. src/usrtools_usrconf/uc_cache.inc
. src/usrtools_usrscr/us_pp.inc
._hooks:global
._hooks:load
#cache_loadmaps "${US_PP_STATE:?}" us_pp_{name_map,meta_static} &&
. src/usrtools_usrscr/us_os.inc
. src/usrtools_usrscr/us_str.inc
. src/usrtools_usrscr/us_ns.inc
._hooks:global
. src/usrtools_usrscr/us_sh.inc
. src/usrtools_usrscr/us_fmt_inc.inc
