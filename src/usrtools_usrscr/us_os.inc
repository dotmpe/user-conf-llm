# .inc file, see User-Conf or User-Script us-inc for specification.
# 
# Copyright 2026 .mpe  <me@dotmpe.com>
# 
# Distributed under terms of the MIT license.
Operating-System() {
: type group
}

.file-data() {
: param '(cat_f=) ~ File [Grep-Filter] ... # Print file data using grep or cat+grep'
: about "Print file data using grep or cat+grep"
: description "File cat/grep helper that normally only prints non-empty, non-comment data"
: extended "Multimode helper routine to retrieve and regex-filter file lines"
  : XXX 'A. replace cat invocation by configurable command and setup some useful pre-grep filters, ie. with file/line number prefix, or file descriptor fields such as size. B. same for filter command, could switch to sed/awk'
: param '(cat_f=) ~ File [Grep-Filter] ...'
: tag util
: completion 'complete -A file'
: id os-nix-file-data
  [[ $# -le 2 && "${1:-"-"}" = - || -e "${1-}" ]] || return 98
  [[ "${1-}" ]] || set -- "-" "${2-}"
  [[ "${2-}" ]] || set -- "$1" '^\s*(#.*|\s*)$'
  [[ -z "${cat_f-}" ]] && {
    grep -Ev "$2" "$1" || return $?
  } || {
    #shellcheck disable=2086
    cat $cat_f "$1" | grep -Ev "$2"
  }
}

.lookup-append() {
: about "Simple PATH-var helper to append only new, unique instance"
: param "<Directory ...> [<Var=PATH>]"
: extended "Using this helps keeping PATH cleaner, but it doesnt behave \
      like path_append but returns false (1) if already found"
: notes TODO "Really should write sys-wordv-add or something"
: notes "This does not export the variable"
: notes "This requires the variable name, use _OS_Path_* alternatively"
: tag util
: completion 'complete -A directory'
  (($#-1)) || return ${_E_MA:-194}
  local _PATHNAME=${@:$#:1}
: input ${_PATHNAME:?$FUNCNAME: Lookup variable, $ENV_CTX}
  local -n _PATH=$_PATHNAME
  #: input "${_PATH?$FUNCNAME: Lookup variable ${_PATHNAME@Q} value, $ENV_CTX}"
  local _arg
  for _arg in "${@:1: $#-1}"
  do
    : input "${_arg:?$FUNCNAME: Value for lookup $_PATHNAME, $ENV_CTX:}"
    case ":${_PATH:-:}:" in
      ( *:"${_arg}":*) ((${assert:-0})) || return 1
      ;;
    ( * )
        _PATH="${_PATH:+${_PATH}:}${_arg}"
    esac
  done
}

.path-append() {
: about "Simple PATH helper to append only new, unique instance"
: param "<Directory ...> [<Var=PATH>]"
: extended "Using this helps keeping PATH cleaner, but it doesnt behave \
      like path_append but returns false (1) if already found"
: notes TODO "Really should write sys-wordv-add or something"
: notes "This does not export PATH"
: notes "This does not require explicit PATH name for single dir argument, and last argument can always be left empty for default"
: notes "Not exactly like the same implementation as shipped with Debian/Ubuntu, see -assert variant"
: tag util
: completion 'complete -A directory'
  local dirc _var
  ! (($#-1)) && dirc=1 || dirc=$#-1 _var=${@: $#:1}
  .lookup-append "${@:1: $dirc}" "${_var:-PATH}"
}

.path-find() {
: param '<Lookup> <Path> <Dest> ...'
  (($#>=3)) || return ${_E_MA:-194}
: input "${1:?$FUNCNAME: Lookup var}"
: input "${2:?$FUNCNAME: Path value}"
: input "${3:?$FUNCNAME: Dest var}"
: ouput '<Lookup>_arr'
: tag util
  local -n _us_os_pf_lookup=${1} _us_os_pf_dest=${3} _us_os_pf_larr=${1}_arr
  local _us_os_pf_root
  if [[ ! ${_us_os_pf_larr[*]:+set} ]]; then
    [[ ${_us_os_pf_lookup:+set} ]] &&
    read -ra ${1}_arr <<< "${_us_os_pf_lookup:?}" ||
      failerr "Empty lookup path at ${1@Q}" || return
  fi
  for _us_os_pf_root in "${_us_os_pf_larr[@]}"; do
    if [[ -e ${_us_os_pf_root}/${2} ]]; then
      _us_os_pf_dest=${_us_os_pf_root}/${2}
      return
    else
      continue
    fi
  done
  false
}

.realpath() {
: input "${1:?$FUNCNAME: Path var}"
: tag util
: completion 'complete -A directory -A file'
  local -n _realpath_ref=${1}
  # XXX: can we do second argument <at> for relative?
  [[ ${_realpath_ref:0:1} = / ]] || {
    #[[ ${_realpath_ref: -1:1} = / ]] && _realpath_ref=${_realpath_ref:0: -1}
    _realpath_ref=$CWD/$_realpath_ref
  }
}

.require-scripts() {
: extended 'Build name to script map via PATH-like lookup plus map globals'
: param '~ <Colon-path-var> <Hash-map> <Sources...> <Status-map>'
: tag util
: completion 'complete -A variable'
  local -I US_SCR_{PATH,HASH}
  : "${US_SCR_PATH:=SCRIPTPATH}"
  : "${US_SCR_HASH:=_os_script_path}"
  local -n _32591e26_pathvar1=${1:-$US_SCR_PATH} _32591e26_hash1=${2:-$US_SCR_HASH}
  local -n _32591e26_src1='_32591e26_hash1["$_scr"]'
  local -a paths
  local _scr
  for _scr in "${@:3: $#-3}"
  do
    ((${#_scr})) || return ${_E_GAE:-193}
    [[ ${_32591e26_src1:+set} ]] ||
      .script-find "${!_32591e26_pathvar1}" "${!_32591e26_src1}" "$_scr" ${US_SCR_EXT-} ||
        failerr "E$? finding required script $_scr" || return
    paths+=( "$_32591e26_src1" )
  done
  ((${#paths[*]})) || return ${_E_MA:-194}
  .source-once "${@: -1:1}" "${paths[@]}"
}

.script-find() {
: param ' ~ <Colon-path-var> <Dest-var> <Basename> [<Exts...>]'
: input "${1?$FUNCNAME${*:+ $*}: Lookup path}"
: input "${2:?$FUNCNAME${*:+ $*}: Destination varname}"
: input "${3:?$FUNCNAME${*:+ $*}: Basename}"
: tag util
  local -I US_SCR_PATH
  : "${US_SCR_PATH:=SCRIPTPATH}"
  local -n _32591e26_pathvar2=${1:-$US_SCR_PATH}
  local -n _32591e26_src3=${2}
  [[ $# -lt 4 ]] &&  {
    # No name extension tests needed, can use command -v / which directly
    ..Core.pass-status "$(PATH=${_32591e26_pathvar2} command -v "$3")" &&
    _32591e26_src3=$_ || return ${_E_not_found:-127}
  } || {
    PATH=${_32591e26_pathvar2} .source-find "${2}" "${@:3}" ||
      return ${_E_not_found:-127}
  }
}

.source-find() {
: param ' ~ <Dest-var> <Basename> <Exts...>'
: tag util
: completion 'complete -A variable'
: input "${1?$FUNCNAME: Varname}"
: input "${2?$FUNCNAME: Basename}"
: input "${3:?$FUNCNAME: Name extension(s)}"
  local _32591e26_{src3,ext3}
  local -n _32591e26_path3=${1:?}
  for _32591e26_ext3 in "${@:3}"
  do
    _32591e26_src3=$(builtin command -v "${2}${_32591e26_ext3}") || continue
    _32591e26_path3=$_32591e26_src3
    return
  done
  #return ${_E_nss:-65}
  return 127
}

.source-once() {
: about 'Source files not yet (successfully) loaded'
: extended 'Tracks source status per script path name via global env'
: param ' <Source> | <Status-map> <Sources...> '
: tag util
: completion 'complete -A file -A arrayvar'
  (($#)) || return ${_E_MA:?}
  # Use parent env if provided, but do not leave global for default value
  local -I US_SCR_STAT
  : "${US_SCR_STAT:=_os_script_load}"
  # Access status cache using by-name variable referencing
  local -n _stats status='_stats["$_scr"]'
  local _scr o=1 s=0
  ! (($#-1)) && _stats=$US_SCR_STAT || o=2 _stats=${1:-$US_SCR_STAT}
  for _scr in "${@:o}"
  do
    [[ ${_scr:+set} ]] || return ${_E_GAE:?}
    # Skip if status is set but empty or actually equals 0
    [[ ${status+set} ]] && {
      [[ ${status:-0} -eq 0 ]] && continue
    }
    builtin . "$_scr"
    status=$?
    # Track first failure status but always keep going, try source all arguments
    ((s)) || [[ ${status:-0} -eq 0 ]] || s=${status}
  done
  return ${s}
}

# Id: us_os                          vim:set ft=bash sw=2 sts=2 et:
