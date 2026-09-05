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
: env "${US_NS_STATE_MAPSPEC:=us_ns_{map{0..3\}\}}"
  declare -ga us_ns_state_assocs
: env "${UC_NS_PATHS:=${UC_NS_PATHS:-.}}"
  cache_load "/var/local/statusdir/meta,loader,uc.data.bash" &&
  ..Operating-System.path-append "${C_INC:?}" "${US_BIN:?}" "${U_S:?}" "${U_C:?}" "${UCONF:?}" UC_NS_PATHS
}
._hooks:load() {
  \builtin read -ra us_ns_state_assocs < <(
    \builtin . <(echo echo ${US_NS_STATE_MAPSPEC:?}))
  ..Cache.load-maps "${US_NS_STATE:?}" "${us_ns_state_assocs[@]:?}"
}

.map-to-ns1() {
: about 'Map each element in path according to flat translation maps'
: param '<Var-name> [<Dest-name>] ...'
: tag util
: input "${1?$(:argv-err 1 'Var name expected')}"
  local -n _uc_ldr_m_map0='uc_meta_ns0["$_uc_ldr_m_node"]'
  local -n _uc_ldr_m_map1='uc_meta_ns0_1_1["$_uc_ldr_m_node"]'
  local -n _uc_ldr_m_ref=${1}
  [[ ${_uc_ldr_m_ref:+set} ]] || failerr "No value to map at ${1@Q}" || return
  _uc_ldr_m_ref="${_uc_ldr_m_ref#/}"
  #shellcheck disable=2206
  local _uc_ldr_m_path=( ${_uc_ldr_m_ref//['/.']/ } )
  local _uc_ldr_m_{node,nid,tmp}
  for _uc_ldr_m_node in "${_uc_ldr_m_path[@]}"; do
    if [[ ${_uc_ldr_m_map1:+set} ]]; then
      _uc_ldr_m_node=${_uc_ldr_m_map1}
    fi
    if [[ ${_uc_ldr_m_map0:+set} ]]; then
      _uc_ldr_m_node=${_uc_ldr_m_map0}
    fi
    : "${_uc_ldr_m_node//[^A-Za-z0-9_]/_}"
    _uc_ldr_m_id=${_,,}
    _uc_ldr_m_tmp+=( "${_uc_ldr_m_id:?}" )
  done &&
  ..String.join-array ${2:-${1}} _uc_ldr_m_tmp _
}

# Id: us_ns                                      vim:set ft=bash sw=2 sts=2 et:
