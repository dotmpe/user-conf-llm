# .inc file, see User-Conf or User-Script us-inc for specification.
#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
Namespace() {
: type group
}

._hooks() { : -A : ; }
._hooks:global() {
# XXX: example layout for data module with hashtables sequence in cache
: env "${US_NS_CACHE:=${CACHE_DIR:-.local/cache}}"
: env "${US_NS_DATADIR:=${USER_DATA_DIR:-.local/user/data}}"
: env "${US_NS_STATE:=${US_NS_DATADIR:?}/us_ns_state.data.bash}"
}
._hooks:load() {
  ..Cache.load-file "${US_NS_STATE:?}" # us_{mod,pack}_{names,name_to_ns}
}

.map-to-ns1() {
: about 'Map each element in path according to flat translation maps'
: param '<Var-name> [<Dest-name>] ...'
: tag util
: input "${1?$(:argv-err 1 'Var name expected')}"

  local -n us_ns_map0='us_pack_ns0_names["$us_ns_node"]'
  local -n us_ns_ref=${1}
  [[ ${us_ns_ref:+set} ]] || failerr "No value to map at ${1@Q}" || return

  #shellcheck disable=2206
  local us_ns_path=( ${us_ns_ref//['/.']/ } )
  local us_ns_{node,nid,tmp}
  for us_ns_node in "${us_ns_path[@]}"; do
    if [[ ${us_ns_map0:+set} ]]; then
      us_ns_node=${us_ns_map0##* }
    fi
    : "${us_ns_node//[^A-Za-z0-9_]/_}"
    us_ns_id=${_,,}
    us_ns_tmp+=( "${us_ns_id:?}" )
  done &&
  ..String.join-array ${2:-${1}} us_ns_tmp _
}

# Id: us_ns                                      vim:set ft=bash sw=2 sts=2 et:
