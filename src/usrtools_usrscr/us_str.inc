String() {
: type group
}

.case-insensitive-glob() {
: param '~ <Glob-var> [<Glob-pattern-value>]'
: about 'Build glob for given glob that is completely case-insensitive'
: extended 'This is only for ASCII, unicode support would be more involved'
: input "${1:?$FUNCNAME${*:+ ${*@Q}}: Glob variable, $ENV_CTX}"
  local -n _us_str_glob1=${1}
  [[ ${2:+set} ]] && _us_str_in1=${2} || _us_str_in1=${_us_str_glob1}
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
: id str/globmatch
: input "${1:?$FUNCNAME${*:+ ${*@Q}}: Pattern, $ENV_CTX}"
: input "${2:?$FUNCNAME${*:+ ${*@Q}}: String value, $ENV_CTX}"
  case "${2}" in ( ${1} ) ;; ( * ) false; esac
}

.globreverse() {
: about 'Bash quick reverse of concatenated strings'
: param ' ~ <Split-match> <Var-name> ...'
  : XXX 'Splits can be character ranges, but then those should reverse properly'
: id str/globreverse
: input "${1:?$FUNCNAME: Split match}"
: input "${2:?$FUNCNAME:$1: Variable name}"
  local -n __globreverse_ref="${2}"
  User-Script.String.globreverse-from "${1}" "${__globreverse_ref}" "${2}"
}

.globreverse-from() {
: about 'Bash quick reverse of concatenated strings'
: param ' ~ <Split-match> <From-value> <To-var> ...'
: input "${1:?$FUNCNAME: Split match}"
: input "${2:?$FUNCNAME:$1: Variable name}"
  local __globreverse_tmp=${2:?$FUNCNAME:$1: Value to reverse}
  local -n __globreverse_to="${3:?}"
  local __globreverse_val
  while User-Script.String.globmatch "*${1}*" "$__globreverse_tmp"
  do
    : "${__globreverse_tmp##*${1}}"
    __globreverse_val=${__globreverse_val:+${__globreverse_val}${1}}${_}
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
  local -n __globreverse_tr_to=${4}
  User-Script.String.globreverse-from "${1}" "${3}" "${4}" &&
  __globreverse_tr_to=${__globreverse_tr_to//$1/$2}
}

.globstrip-charsleft() {
: param '~ <String-name> [<Match-expression>] ...'
  (($#)) || return ${_E_MA:?}
: input "${1:?$FUNCNAME${*:+ ${*@Q}}: String name expected, $ENV_CTX}"
  local -n _us_gs_cl_str=${1:?}
: input "${_us_gs_cl_str:?$FUNCNAME${*:+ ${*@Q}}: String value expected, $ENV_CTX}"
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
: input "${1:?$FUNCNAME${*:+ ${*@Q}}: String value expected, $ENV_CTX}"
  local _us_gs_cl_str=${1}
  local _us_gs_cl_prefc=${2:-"[ ]"}

  \inline ..inner
  echo "$_us_gs_cl_str"
}

.globstrip-charsright() {
  : param '~ <String> [<Match-expression>] ...'
  (( $#)) || return ${_E_MA:?}
  : input "${1:?$FUNCNAME${*:+ ${*@Q}}: String value expected, $ENV_CTX}"
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

.join() {
: param '<Dest> <Sep> <Str...>'
: about 'Wrapper for join-array to insert via arguments'
: completion 'complete -A arrayvar'
  local -a _us_str_a1=( "${@:3}" )
  User-Script.String.join-array "${1}" _us_str_a1 "${2}"
}

.join-array() {
: param '<Dest> <Src-arr> [<Sep>] ...'
: completion 'complete -A arrayvar'
  local -n _us_str_d2=${1-}
  local -n _us_str_a2=${2-}
  local _us_str_s2
  _us_str_d2=${_us_str_a2[0]}
  for _us_str_s2 in "${_us_str_a2[@]:1}"
  do
    _us_str_d2+=${_us_str_d2:+${3-}}${_us_str_s2}
  done
}

.line-slice() {
: param '~ <Column-offset> <Columns-width> ...'
: input "${1?$FUNCNAME${*:+ ${*@Q}}: Slice offset}"
: input "${2?$FUNCNAME${*:+ ${*@Q}}: Slice size}"
  local start=${1:-0} len=${2} line
  while read -r line
  do
    [[ ${len:+set} ]] &&
    echo "${line: $start:$len}" ||
      echo "${line: $start}"
  done
}

.line-surround() {
: param '~ Prefix Suffix [Dest] [Input]'
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
: about ''
: param '~ <Pattern-map> <Variable> [<Destination-variable>]'
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
: input "${1:?$FUNCNAME${*:+ ${*@Q}}: Target variable name}"
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
: input "${1:?$FUNCNAME${*:+ ${*@Q}}: Target variable name}"
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
: input "${3:?$FUNCNAME${*:+ ${*@Q}}: Command line}"
: completion 'complete -F _command'
  if_ok "$("${@:3}")" &&
  .line-slice "${@:1:2}" <<< "$_"
}

.wordmatch-any() {
: param '~ <Word> <Words...>'
: about 'Non-zero unless literal Word or pattern match is found in Words'
: input "${1:?$FUNCNAME${*:+ $*}: Literal word or pattern expected}"
: input "${2:?$FUNCNAME${*:+ $*}: Word values expected}"
  # Match Word with Strings, expanding string expression in Word
  case " ${*:2} " in ( *\ ${1}\ * ) ;; ( * ) false; esac
}

# TODO: replace with compiled part from str,us
# Id: us-str.sh
