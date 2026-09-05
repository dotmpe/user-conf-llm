# .inc file, see User-Conf or User-Script us-inc for specification.
#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
Cache() {
: type group
  #_als=(
  #  [cache_loadmaps]=.load-maps
  #)
}
._hooks() { :- : -A : ; }
._hooks:global() {
: env "${UC_CACHE_DIR:=${CACHE_DIR:-.local/cache}}"
: env "${UC_CACHE_STATE:=${UC_CACHE_DIR:?}/uc_cache.data.bash}"
}
._hooks:load() {
  ! [[ ${UCACHE:+set} ]] || {
    test -d "$HOME/.local/var/uc/cache" && UCACHE=$_
    test -d "$HOME/.cache/local/uc" && UCACHE=$_
    test -d "$UCACHE" || >&2 mkdir -vp "$UCACHE"
  }
}

.define-object() {
: TODO unused, adapt to uc_meta setup somehow.
: param ' ~ <Data-file> <Source-prefix> <Index1> <Suffixes...> [ -- <IndexN> <Suffixes...> ]'
: input "${1?$(:argv-err 1 'Shell script cache file')}"
: input "${2?$(:argv-err 2 'Source prefix symbol')}"
: input "${3?$(:argv-err 3 'Source index expression')}"
: input "${4?$(:argv-err 4 'Source suffixes')}"
  local datafile=${1} target=${2}
  local -n __fields=${target}_fields
  local -n __namerefs=${target}_namerefs
  local -n __names=${target}_names
  local -n __srcsyms=${target}_srcsyms
  ..Object.declare "${@:2}" &&
  .set-source "$datafile" "${__srcsyms[@]}"

  declare -p ${!__srcsyms}
}

.delete() {
: param ' ~ <File> <Key-literal...>'
: tag cache data
  local __uc_cache_file=${1:?}
  { printf 'unset'; printf ' "%s"' "${@:2}" ; printf '\n'
  } >> "${__uc_cache_file}"
}

.delete-refs() {
: param ' ~ <File> <Key-reference...>'
: tag cache data
  (($#)) || return ${_E_MA:?}
: input "${1?$(:argv-err 1 'Cache file')}"
: input "${2?$(:argv-err 2 'Map key(s)')}"
  local __uc_cache_file=${1:?} keys
  \builtin . <(echo "keys=($(printf ' "%s"' "${@:2}") )")
  { printf 'unset'; printf ' "%s"' "${keys[@]}" ; printf '\n'
  } >> "${__uc_cache_file}"
}

.load-file() {
: param '~ <Data-file> ...'
: tag util aux
: TODO: "move this to uc-meta to make it work more clearly, but until then need this as user helper"
  (($#)) || return ${_E_MA:?}
: input "${1?$(:argv-err 1 'Data file')}"
  [[ -s "${1}" ]] &&
  \builtin . "${1}" && {
    ! ((VERBOSE)) || {
      # Print summary of loaded bytes/items
      :pass "$(du -hs "${1}")" &&
      : "${_%%$'\t'*}" &&
      >&2 echo "Cache loaded ($_ bytes)" || : "???"
    }
  } ||
    ! ((VERBOSE)) ||
      >&2 echo "Missing or empty ${1@Q} cache (E$?, ignored)"
}

.load-maps() {
: param '~ <Data-file> <Map-exports...>'
: about 'Helper to retrieve map arrays from Shell cache file'
: tag cache
: input "${1?$(:argv-err 1 'Shell script cache file')}"
: input "${2?$(:argv-err 2 'Associative array name(s)')}"
  declare -gA "${@:2}" ||
    :failerr "Cannot declare global maps ${*@Q}" || return
  [[ -s "${1}" ]] &&
  \builtin . "${1}" && {
    ! ((VERBOSE)) || {
      # Print summary of loaded bytes/items
      :pass "$(du -hs "${1}")" &&
      : "${_%%$'\t'*}" &&
      say.info "Cache loaded ($_ bytes) for ${*:2}" || :
      local -n _ref
      for _ref in "${@:2}"; do
        [[ ! "${_ref[*]:+set}" ]] ||
          say.v "Found ${#_ref[@]} ${!_ref} items in cache"
      done
    }
  } || say.err "Missing or empty $1 cache (E$?, ignored)"
}

.rekey-maps() {
: param ' ~ <Oldcache> <Newcache> <Prefix> <Oldkey> <NewKey> <Fields>'
  (($#)) || return ${_E_MA:?}
  local field
  [[ ! -s "$2" ]] || \builtin . "$2" || return
  for field in "${@:6}"; do
    local -n _old="${3}_${field}[\"${4}\"]"
    [[ ${_old+set} ]] ||
      :failerr "no data for $3 $4 $field" || continue
    local -n _new="${3}_${field}[\"${5}\"]"
    [[ ${_old} = "${_new-}" ]] ||
    .update-map "$2" "${!_new}" "${_old}" || return
  done
}

.set-index() {
: about ' ~ <File> <Keyname> <Data> ...'
: tag cache data
  (($#)) || return ${_E_MA:?}
: input "${2?$(:argv-err 2 'Index position')}"
  local -n __target=${2}
  [[ ${__target:+set} && "$__target" = "$3" ]] && return
  .update-index "$@" &&
  __target=${3}
}

.set-map() {
: about ' ~ <File> <Keyname> <Data> ...'
: about 'Helper to generate correct cache key name and update current env value'
: tag cache data
  (($#)) || return ${_E_MA:?}
: input "${2?$(:argv-err 2 'Map key')}"
  local -n __target=${2}
  [[ ${__target:+set} && "$__target" = "$3" ]] && return
  .update-map "$@" &&
  __target=${3}
}

.set-source() {
: about ' ~ <File> <Keynames...>'
: tag cache file
: input "${1?$(:argv-err 1 'Data file path')}"
: input "${2?$(:argv-err 2 'Data symbol(s)')}"
  local file=${1} sym uc_cache_{fields,source,name{ref,key}s,srcsyms}
  ..Object.declare uc_cache \$sym source &&
  \builtin . <(echo "local -n ${uc_cache_namerefs[*]:?}") &&
  to-v declare -p uc_cache_{fields,source,name{ref,key}s,srcsyms} &&
  .load-maps "$UC_CACHE_STATE" "${uc_cache_srcsyms[@]:?}" || return
  shift
  for sym; do
    .set-map "$UC_CACHE_STATE" "${!__source:?}" "$file"
  done
}

.update-index() {
: param ' ~ <File> <Key-spec> <Data> ...'
: tag cache data
: input "${1?$(:argv-err 1 'Cache file')}"
: input "${2?$(:argv-err 2 'Index offset integer')}"
: input "${3?$(:argv-err 3 'Data')}"
  local idxspec idx

  # Strip array name from idx part
  : "${2#*[}"
  : "${_%]}"
  : "${_%\"}"
  idxspec=${_#\"}

  # expand any variable reference in idx part
  : "${idxspec//\"/\\\"}"
  :pass "$(\builtin . <(echo "echo \"${_}\""))" &&
  idx=${_} &&
  echo "${2%\[*}[${idx}]=${3@Q}" >> "${1}"
}

.update-map() {
: param ' ~ <File> <Keyname> <Data> ...'
: about 'Helper to generate correct cache key name as quoting is a bit of a pain'
: extended 'To update env as well, see .set-map'
: tag cache data
: input "${1?$(:argv-err 1 'Cache file')}"
: input "${2?$(:argv-err 2 'Map key')}"
: input "${3?$(:argv-err 3 'Data')}"

  # XXX want to do some encoding to ensure both variables (key and value) are
  # oneline strings, so using $'...' to that effect just like declare -p does
  local keyspec key

  # Strip array name from key part
  : "${2#*[}"
  : "${_%]}"
  : "${_%\"}"
  keyspec=${_#\"}

  # expand any variable reference in key part
  : "${keyspec//\"/\\\"}"
  :pass "$(\builtin . <(echo "echo \"${_}\""))" &&
  key=${_} &&
  echo "${2%\[*}[\$${key@Q}]=${3@Q}" >> "${1}"
}

# Id: uc_cache                                   vim:set ft=bash sw=2 sts=2 et:
