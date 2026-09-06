#!/bin/bash

scr_pre=tool/local
\builtin . $scr_pre/common_script.bash
\builtin . $scr_pre/common-dsl.bash

usp uc-cache
cache_loadmaps "${US_PP_STATE:?}" us_pp_{name_map,meta_static}

printf.lines.array-map.tab us_pp_name_map
#printf.lines.array-map.tab us_pp_meta_static
