#!/usr/bin/env bash
set -euo pipefail
shopt -s extdebug
IFS=$' \t\n'

# TODO: want to have several base envs for testing; bc on dev env everything
# can be inherited that is where test env starts as well
if [[ ${US_BBB_ENV:+set} ]]; then

  : # XXX: dynamic reinit (arrays etc.)?
  #. <(printf '%s\n' "$US_BBB_ENV")

elif [[ ${US_ENV_INIT:+set} ]]; then

  declare -gA _os_script_{load,path}
  METADIR=.local
  us-env -R us-env
  us_part --alias --export --hooks:define,declare,init us-bbb uc-cache
else
  >&2 echo "No shell profile support"
  exit 1
fi

. "test_common.bash"
