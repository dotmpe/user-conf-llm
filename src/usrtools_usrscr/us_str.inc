String() {
: type group
}

.case-insensitive-glob() {
: param '~ <Glob-var> [<Glob-pattern-value>] ...'
: about 'Build glob for given glob that is completely case-insensitive'
: extended 'This is only for ASCII, unicode support would be more involved'
: input "${1:?$(:argv-err 1 'Glob variable')}"
  local -n _us_str_glob1=${1}
  [[ ${2:+set} ]] && _us_str_in1=${2} || _us_str_in1=${_us_str_glob1}
: input "${_us_str_in1:?$(:unset-err $_us_str_in1 "String value")}"
  _us_str_glob1=
  while read -r -n 1 _us_str_char1
  do
    case "$_us_str_char1" in
    ( [A-Za-z] ) _us_str_glob1+="[${_us_str_char1^}${_us_str_char1,}]" ;;
    ( * ) _us_str_glob1+="${_us_str_char1}"
    esac
  done <<< "$_us_str_in1"
}

.globmatch() {
: about "Inline glob match of String to Pattern"
: param " ~ <Pattern> <String> ..."
: input "${1:?$(:argv-err 1 'Pattern')}"
: input "${2:?$(:argv-err 2 'String name')}"
  local -n _gm_str_name=${2}
: input "${_gm_str_name:?$(:unset-err $2 "String value")}"
  #shellcheck disable=2254  # Unquoted var is meant as case/esac glob expansion
  case "${_gm_str_name}" in ( ${1} ) ;; ( * ) false; esac
}

.globmatch.str() {
: about "Inline glob match of String to Pattern"
: param " ~ <Pattern> <String> ..."
: id str/globmatch
: input "${1:?$(:argv-err 1 'Pattern')}"
: input "${2:?$(:argv-err 2 'String value')}"
  #shellcheck disable=SC2254  # Unquoted var is meant as case/esac glob expansion
  case "${2}" in ( ${1} ) ;; ( * ) false; esac
}

.globreverse() {
: about 'Bash quick reverse of concatenated strings'
: param ' ~ <Split-match> <Var-name> ...'
  : XXX 'Splits can be character ranges, but then those should reverse properly'
: id str/globreverse
: input "${1:?$(:argv-err 1 'Split match')}"
: input "${2:?$(:argv-err 2 'String name')}"
  local -n __globreverse_ref="${2}"
  .globreverse-from "${1}" "${__globreverse_ref}" "${2}"
}

.globreverse-from() {
: about 'Bash quick reverse of concatenated strings'
: param ' ~ <Split-match> <From-value> <To-var> ...'
: input "${1:?$(:argv-err 1 'Split match')}"
: input "${2:?$(:argv-err 2 'Source variable name')}"
: input "${3:?$(:argv-err 3 'Destination variable name')}"
  local __globreverse_tmp=${2}
  local -n __globreverse_to=${3}
  local __globreverse_val
  while
    .globmatch "*${1}*" "$__globreverse_tmp"
  do
    #shellcheck disable=SC2295  # Match expansion from var is the idea here
    : "${__globreverse_tmp##*${1}}"
    __globreverse_val=${__globreverse_val:+${__globreverse_val}${1}}${_}
    #shellcheck disable=SC2295  # Match expansion from var is the idea here
    __globreverse_tmp="${__globreverse_tmp%${1}*}"
  done
  [[ -z "${__globreverse_tmp-}" ]] || {
    __globreverse_val=${__globreverse_val:+${__globreverse_val}${1}}${__globreverse_tmp}
  }
  __globreverse_to=$__globreverse_val
}

.globreverse-replace() {
: about 'Reverse at and replace separator'
: param ' ~ <Split-match> <New-sep> <Value> <Var-to> ...'
: input "${1:?$(:argv-err 1 'Split match')}"
: input "${2:?$(:argv-err 2 'New separator')}"
: input "${3:?$(:argv-err 3 'String value')}"
: input "${4:?$(:argv-err 4 'Destination variable name')}"
  local -n __globreverse_tr_to=${4}
  .globreverse-from "${1}" "${3}" "${4}" &&
  __globreverse_tr_to=${__globreverse_tr_to//$1/$2}
}

.globstrip-charsleft() {
: param '~ <String-name> [<Match-expression>] ...'
: input "${1:?$(:argv-err 1 'String name')}"
  local -n _us_gs_cl_str=${1:?}
: input "${_us_gs_cl_str:?$(:unset-err $_us_gs_cl_str "String value")}"
  local _us_gs_cl_prefc=${2:-"[ ]"}

  \inline .inner
}

.globstrip-charsleft.inner() {
  while
    .globmatch "$_us_gs_cl_str" "$_us_gs_cl_prefc*"
  do
    #shellcheck disable=SC2295  # Match expansion from var is the idea here
    _us_gs_cl_str="${_us_gs_cl_str#$_us_gs_cl_prefc}"
    [[ "$_us_gs_cl_str" ]] || break
  done
}

.globstrip-charsleft.str() {
: param '~ <String-value> [<Match-expression>] ...'
: input "${1:?$(:argv-err 1 'String value')}"
  local _us_gs_cl_str=${1}
  local _us_gs_cl_prefc=${2:-"[ ]"}

  \inline ..inner
  echo "$_us_gs_cl_str"
}

.globstrip-charsright() {
: param '~ <String> [<Match-expression>] ...'
: input "${1:?$(:argv-err 1 'String value')}"
  local sufc=${2:-"[ ]"} str="${1}"
  while
    .globmatch "$str" "*$sufc"
  do
    #shellcheck disable=SC2295  # Match expansion from var is the idea here
    str="${str%$sufc}"
    [[ "$str" ]] || break
  done
  echo "$str"
}

.join-str() {
: param '~ <Dest> <Sep> <Str...>'
: about 'Wrapper for join-array to insert via arguments'
: completion 'complete -A arrayvar'
: input "${1:?$(:argv-err 1 'Destination name')}"
: input "${2:?$(:argv-err 2 'Separator value')}"
: input "${3:?$(:argv-err 3 'String value(s)')}"
  local -a _us_str_a1=( "${@:3}" )

  .join-array "${1}" _us_str_a1 "${2}"
}

.join-array() {
: param '~ <Dest> <Src-arr> [<Sep>] ...'
: completion 'complete -A arrayvar'
: input "${1:?$(:argv-err 1 'Destination name')}"
: input "${2:?$(:argv-err 2 'Source name')}"
  local -n _us_str_d2=${1}
  local -n _us_str_a2=${2}
  local _us_str_s2

  _us_str_d2=${_us_str_a2[0]}
  for _us_str_s2 in "${_us_str_a2[@]:1}"
  do
    _us_str_d2+=${_us_str_d2:+${3-}}${_us_str_s2}
  done
}

.line-slice() {
: param '~ <Column-offset> <Columns-width> ...'
: input "${1?$(:argv-err 1 'Column offset')}"
: input "${2:?$(:argv-err 2 'Columns width')}"
  local start=${1:-0} len=${2} line

  while read -r line; do
    [[ ${len:+set} ]] &&
    echo "${line: $start:$len}" || echo "${line: $start}"
  done
}

.line-surround() {
: param '~ Prefix Suffix [Dest] [Input] ...'
: input "${1:?$(:argv-err 1 'Prefix value')}"
: input "${2:?$(:argv-err 2 'Suffix value')}"
  local _us_str_ls_in __us_str_dest
  local -n _us_str_ls_dest=${3:-__us_str_dest}
  # XXX: use dest as optional source perhaps
  if [[ ${4:+set} && $4 != - ]]; then
    _us_str_ls_in=$4
  else
    _ read -rd '' _us_str_ls_in
  fi

  [[ ${_us_str_ls_in:+set} ]] ||
    failerr "$FUNCNAME: Empty input" || return
  : "${_us_str_ls_in}"
  : "${1}${_//$'\n'/$'\n'${1}}"
  : "${_//$'\n'/${2}$'\n'}${2}"
  _us_str_ls_dest=$_
  [[ ${!_us_str_ls_dest} != __us_str_dest ]] || echo "$_us_str_ls_dest"
}

.map-normalize() {
: param '~ <Pattern-map> <Variable> [<Destination-variable>] ...'
  local _us_str_mapname=${1:?} _us_str_varname=${2:?} _us_str_outname=${3:-$2}
  local -n _us_str_varref=${2} _us_str_outref=${3:-$2}
  if sh_fun "$1"; then
    . <($1)
  elif sh_fun "$FUNCNAME$1"; then
    . <(sh_funbody $FUNCNAME$1)
  elif [[ $1 =~ [A-Za-z_][A-Za-z0-9_]* ]]; then
    TODO "generate case/esac and cache, or iterate map" || return
  else
    TODO "unknown map format ${1@Q}"
  fi
}

.map-normalize--boolean() {
: tag part
  case "${_us_str_varref,,}" in
  ( y|j|true|on|1 ) _us_str_outref=${US_STR_TRUE:-true} ;;
  ( n|f|false|off|0 ) _us_str_outref=${US_STR_FALSE:-false} ;;
    * ) failerr "Not a mapped boolean value: ${_us_str_varref@Q}"
  esac
}

.name-concat-all-right() {
: param '~ <Destination-variable> <Strings...>'
: about 'Add string to right, use STR_FS if <Variable> is non-empty'
: extended 'STR_FS or String can still be left empty. Proper input and context is left to user. '
: input "${1:?$(:argv-err 1 'Destination variable')}"
: input "${2:?$(:argv-err 2 'String value(s)')}"
: id shell/sys/_concat-if-ne/name-concat-strings-right
: alias -nconcats
  declare -n __name_concat1=$1
  shift
  local __name_concat2
  for __name_concat2; do
    __name_concat1="${__name_concat1-}${__name_concat1:+${STR_FS- }}${__name_concat2}"
  done
}

.name-concat-names-right() {
: param '~ <Destination-variable> <Variable...>'
: input "${1:?$(:argv-err 1 'Destination variable')}"
: input "${2:?$(:argv-err 2 'Source variable(s)')}"
: id shell/sys/_concat-if-ne/name-concat-names-right
: alias -nconcatn
  declare -n __name_concat1=$1
  shift
  local -n __name_concat2
  for __name_concat2; do
    __name_concat1="${__name_concat1-}${__name_concat1:+${STR_FS- }}${__name_concat2}"
  done
}

.output-slice() {
: param '~ <Start> <Len> <Command...>'
: about 'Slice output lines of subcommand'
: input "${1:?$(:argv-err 1 'Start')}"
: input "${2:?$(:argv-err 2 'Length')}"
: input "${3:?$(:argv-err 3 'Command line')}"
: completion 'complete -F _command'
  :pass "$("${@:3}")" &&
  .line-slice "${@:1:2}" <<< "$_"
}

.wordmatch-any() {
: param '~ <Word> <Words...>'
: about 'Non-zero unless literal Word or pattern match is found in Words'
: input "${1:?$(:argv-err 1 'Literal word or pattern')}"
: input "${2:?$(:argv-err 2 'Word value(s)')}"
  # Match Word with Strings, expanding string expression in Word
  case " ${*:2} " in ( *\ ${1}\ * ) ;; ( * ) false; esac
}

# Id: us_str                                     vim:set ft=bash sw=2 sts=2 et:
