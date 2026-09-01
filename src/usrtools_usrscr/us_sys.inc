# .inc file, see User-Conf or User-Script us-inc for specification.
# 
# Copyright 2026 .mpe  <me@dotmpe.com>
# 
# Distributed under terms of the MIT license.
System() {
: type group
}

.call-apply() {
: param '<Cmd...> <Sequence>'
: about 'Call Cmd with each <Arg> from array Sequence'
: description 'Like mapcar, but do not store output. Returns on failure, to '
: description 'run complete sequence and only then return first failure, prefix with --all.'
: extended 'Adds one argument to each invocation. To format need other idiom. '
: extended 'To store either status or output per command, XXX: see stat and map'
: extended 'To iterate on commands instead of argument items, see sys-call-narr. '
: extended 'For maps, use call-apply-map to iterate on keys instead and apply both key and value. '
: id sh/sys-call-apply
: oldid sh/sys-exec-apply
: input "${1:?$FUNCNAME${*:+ $*}: Command line expected, $ENV_CTX}"
: input "${*: -1:1}"
: input "${_:?$FUNCNAME${*:+ $*}: Sequence array name expected, $ENV_CTX}"
  local _sys_call_{all,arg,ret,stat} __scao
  [[ "$1" = "--all" ]] && __scao=2 _sys_call_all=1 || __scao=1 _sys_call_all=0
  local -n _sys_call_seq=${*: -1:1}
  for _sys_call_arg in "${_sys_call_seq[@]}"
  do
    "${@:__scao: $#-__scao}" "$_sys_call_arg" || {
      _sys_call_ret=$?
      [[ ${_E_next:?} -eq $_sys_call_ret ]] && continue
      [[ ${_E_break:?} -eq $_sys_call_ret ]] && return
      (( _sys_call_all )) && {
        (( ${_sys_call_stat:-0} )) || _sys_call_stat=$_sys_call_ret
      } || {
        ! (( _sys_call_ret )) || return ${_sys_call_ret}
      }
    }
  done
  return ${_sys_call_stat-0}
}

.call-apply-map() {
: param '<Cmd...> <Map>'
: about 'Call command with <Key> <Value-> <Arrayname> for all keys in hashtable'
: extended 'This is identical to call-apply except it iterates on keys'
: input "${1:?$FUNCNAME${*:+ $*}: Command line expected, $ENV_CTX}"
: input "${*: -1:1}"
: input "${_:?$FUNCNAME${*:+ $*}: Hashtable array name expected, $ENV_CTX}"
  local _sys_call_{all,key,ret,stat} __scao
  [[ "$1" = "--all" ]] && __scao=2 _sys_call_all=1 || __scao=1 _sys_call_all=0
  local -n _sys_call_map=${*: -1:1}
  local -n _sys_call_val='_sys_call_map["$_sys_call_key"]'
  for _sys_call_key in "${!_sys_call_map[@]}"
  do
    "${@:__scao: $#-__scao}" "$_sys_call_key" "${_sys_call_value-}" "${!_sys_call_map}" || {
      _sys_call_ret=$?
      [[ ${_E_next:?} -eq $_sys_call_ret ]] && continue
      [[ ${_E_break:?} -eq $_sys_call_ret ]] && return
      (( _sys_call_all )) && {
        (( ${_sys_call_stat:-0} )) || _sys_call_stat=$_sys_call_ret
      } || {
        ! (( _sys_call_ret )) || return ${_sys_call_ret}
      }
    }
  done
  return ${_sys_call_stat-0}
}

.read-call() {
: about "Read command's standard ouput (lines) into array"
: extended "Reads onto end for existing array. This invokes the command, and "
: extended "then feeds the output to mapfile."
: notes "To read words as items, see _Sys_Read_Items"
: notes "To just read file data "
: param "<Array-name> <Cmd...>"
: id sh/sys-read-call
: group Shell/Sys
: oldid sh/sys-read-exec
: aliases _Sys_Read_Exec _Sh_Read_Exec sys_exec_mapfile
  : XXX maybe call this _Sys_Read_Onto? _Sh_Mapfile
: input "${1:?$FUNCNAME${*:+ $*}: Array-name expected, $ENV_CTX}"
: input "${2:?$FUNCNAME${*:+ $*}: Command line expected, $ENV_CTX}"
  local -n __sys_read_call_arr=${1}
  local outname=${1} offset
  [[ ${__sys_read_call_arr[*]:+set} ]] &&
  offset=${#__sys_read_call_arr[@]} || offset=0
  ..Core.pass-status "$("${@:2}")" || return ${_E_fail:-$?}
  test -n "$_" || return ${_E_nz:-9}
  <<< "$_" mapfile -O ${offset} ${mapfile_f:--t} ${outname}
}

.read-call-to-map() {
: about 'Read output lines as key-value pairs separated by given string'
: param '~ <Hash> <Sep> <Command...>'
: input "${1:?$FUNCNAME${*:+ $*}: Hash name expected, $ENV_CTX}"
: input "${2:?$FUNCNAME${*:+ $*}: Separator string expected, $ENV_CTX}"
: input "${3:?$FUNCNAME${*:+ $*}: Command line expected, $ENV_CTX}"
  local -n _ef95aea5_arr=${1}
  local _ef95aea5_sep=${2} _ef95aea5_{key,val}
  ..Core.pass-status "$("${@:3}")" &&
  test -n "$_" || return 0
  while read -r line
  do
    _ef95aea5_key=${line%%"$_ef95aea5_sep"*}
    _ef95aea5_val=${line: ${#_ef95aea5_key}+${#_ef95aea5_sep}}
    _ef95aea5_arr["$_ef95aea5_key"]=$_ef95aea5_val
  done <<< "$_"
}

# Id: us_sys                          vim:set ft=bash sw=2 sts=2 et:
