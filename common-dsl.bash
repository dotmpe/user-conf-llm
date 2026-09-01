#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.

if [[ ${0##*/} = common-dsl.bash ]]; then
  \builtin . ./common_setup.bash
  \builtin . ./common_env.bash
  shopt -s expand_aliases
  shopt -s extdebug
fi

:_() {
: about 'Placeholder for pseudo-macro to unfold Bash line'
: param '~ <Sub-command...>'
  ! (($#)) || "$@"
}

:argv-err() {
: param '~ <Position> <Label>'
  set -- "${FUNCNAME[1]}" "$2" "${3:-expected }" "${4:-at position }" "$1"
  printf '%s: %s %s%s%i\n' "$@"
}

:say-when() {
  local min_level=${1:-2}
  (( VERBOSITY >= min_level )) || return 0
  printf '%s\n' "${*:2}" >&${USER_FD}
}

:unset-err() {
: param '~ <Symbol> <Label>'
  set -- "${FUNCNAME[1]}" "$2" "${3:-expected }" "${4:-at name }" "$1"
  printf '%s: %s %s%s%s\n' "$@"
}

to-v() {
  "$@" >&${USER_FD}
}

# sometimes when writing it might help to have dev-mode only defs, like this:
if shopt -q expand_aliases; then
  alias printf.line="printf '%s\\n'"
  alias 'say@v=:say-when $VERBOSITY'
  #shellcheck disable=2142  # alias referencing positionals is fine, actually
  alias functxln='say@v "$FUNCNAME: ${*@Q} [$#]"'
else
  printf.line() { printf '%s\\n' "$@"; }
  say@v() { :say-when $VERBOSITY "$1"; }
  functxln() {
    say@v "${FUNCNAME[1]}: $(TODO "bash call arg inspection for outer function?")";
  }
fi
printf.lines() {
  local -n _pfl_arr=${1:?Array name}
  printf.line "${_pfl_arr[@]}"
}
printf.lines.array-map.tab() {
  local _pfl_k
  local -n _pfl_arr=${1:?Array name} _pfl_item='_pfl_arr["$_pfl_k"]'
  shift
  (($#)) || set -- "${!_pfl_arr[@]}"
  for _pfl_k; do
    : "${_pfl_item//$'\n'/$'\n  '}"
    printf '%s\t%s\n' "$_pfl_k" "${_-NULL}";
  done
}
say.err() { :say-when 1 "$1"; }
say.info() { :say-when 2 "$1"; }
say.v() { :say-when 3 "$1"; }
say.debug() { :say-when 4 "$1"; }

# XXX: helpers for user-equiv. for log, temporary until better init
:_debug() {
  ((QUIET)) || ! ((DEBUG)) || :say-when 4 "$1"
}
:_info() {
  ((QUIET)) || :say-when 2 "$@"
}
:_notice() {
  ((QUIET)) || :say-when 3 "$@"
}

# other dev-mode impl. helper, to be stripped/replaced before pack and dist
TODO() {
  (($#)) && : "To-do: $*" || {
    ((${#FUNCNAME[*]} > 1)) && : "${FUNCNAME[1]}()" || : "main"
    : "Unspecified to-do in ${_@Q}"
  }
  ${TODO_call:-say@v} "${_}"
  return ${_E_todo:-125}
}

# TODO: move :* to other "DSL" groups

:_args+names() {
  local -a names
  \builtin . <(printf "names=( %s )" "$1") &&
  [[ ${names[*]:+set} ]] ||
    say.err "Null expansion ${1@Q}" ${_E_usage:-64}
}

# Vanilla bash 'inlining' / definition reuse
# around current sh-funbody

# XXX: real inlining is possible once loader takes over source.
# It also looks a bit nicer. Var ___ is used as 'input' to the Bash alias. And
# _%_ is a sort of recognition of the special alias template scripts' status.

:inline.fun() {
: about 'Include body of function, as-is as source'
  \builtin . <(sh_funbody ${_%_})
}

:inline.fun.status() {
: about 'Include body of function with return status'
  \builtin . <(sh_funbody ${_%_} _fb_script && echo "${_fb_script:?} || return")
}

# TODO: strip (most) : lines in sh_funscr <fun>, or use specific call: sh_funscr_nometa {als,tag,about,type} ...
_inline_fun_tpl=$(sh_funbody :inline.fun)
#shellcheck disable=2139  # var is expanded from tpl on assign
alias inline-fun="${_inline_fun_tpl//_%_/___}"
_inline_fun_status_tpl=$(sh_funbody :inline.fun.status)
#shellcheck disable=2139  # var is expanded from tpl on assign
alias inline-fun-status="${_inline_fun_status_tpl//_%_/___}"


# Utilities for Args module, or to keep with us-arr Arr module as special
# templates / cases for the generic User-Script.Arr.zip-{bind,copy,assign} impl.
# that work on arrays.

:bind-args() {
: param '~ <Expansion> <Variables...>'
  (($#)) || return ${_E_MA:?}
: input "${1:?$(:argv-err 1 'Brace or glob expression')}"
: input "${2:?$(:argv-err 2 'Variable references')}"
  ___=:_args+names; inline-fun-status
  :zip-bind.args names "${@:2}"
}

:copy-args() {
: param '~ <Expansion> <Variables...>'
: input "${1:?$(:argv-err 1 'Brace or glob expression')}"
: input "${2:?$(:argv-err 2 'Source variables')}"
  ___=:_args+names; inline-fun-status
  :zip-copy.args names "${@:2}"
}

:read-args() {
: param '~ <Expansion> <Values...>'
: input "${1:?$(:argv-err 1 'Brace or glob expression')}"
: input "${2:?$(:argv-err 2 'Assignment values')}"
  ___=:_args+names; inline-fun-status
  :zip-assign.args names "${@:2}"
}

:zip-assign.args() {
: param '~ <Array> <Values...>'
: input "${1:?$(:argv-err 1 'Array name')}"
: input "${2:?$(:argv-err 2 'Assignment values')}"
  local -n _za_names=$1
  local i
  for i in "${!_za_names[@]}"; do
    ((i+=2))
    printf -v "${_za_names[i-2]}" '%s' "${!i}"
  done
}

:zip-bind.args() {
: param '~ <Array> <Variables...>'
: input "${1:?$(:argv-err 1 'Array name')}"
: input "${2:?$(:argv-err 2 'Variable references')}"
  local -n _zb_names=$1
  local _zb_{i,global_ref}
  for _zb_i in "${!_zb_names[@]}"; do
    ((_zb_i+=2))
    _zb_global_ref="${!_zb_i}"
    declare -gn "${_zb_names[_zb_i-2]}=${_zb_global_ref}"
  done
}

:zip-copy.args() {
: param '~ <Array> <Variables...>'
: input "${1:?$(:argv-err 1 'Array name')}"
: input "${2:?$(:argv-err 2 'Source variables')}"
  local -n _zc_names=$1
  local _zc_{i,global_ref}
  for _zc_i in "${!_zc_names[@]}"; do
    ((_zc_i+=2))
    _zc_global_ref="${!_zc_i}"
    printf -v "${_zc_names[_zc_i-2]}" '%s' "${!_zc_global_ref}"
  done
}


# Temp copy from str tools (us_str.inc)

:globmatch() {
: about "Inline glob match of String to Pattern"
: param " ~ <Pattern> <String> ..."
: input "${1:?$(:argv-err 1 'Pattern')}"
: input "${2:?$(:argv-err 2 'String name')}"
  local -n _gm_str_name=${2}
: input "${_gm_str_name:?$(:unset-err $2 "String value")}"
  #shellcheck disable=2254  # Unquoted var is meant as case/esac glob expansion
  case "${_gm_str_name}" in ( ${1} ) ;; ( * ) false; esac
}

:globmatch.str() {
: about "Inline glob match of String to Pattern"
: param " ~ <Pattern> <String> ..."
: input "${1:?$(:argv-err 1 'Pattern')}"
: input "${2:?$(:argv-err 2 'String value')}"
  #shellcheck disable=2254  # Unquoted var is meant as case/esac glob expansion
  case "${2}" in ( ${1} ) ;; ( * ) false; esac
}

:globstrip-charsleft() {
: param '~ <String-name> [<Match-expression>] ...'
: input "${1:?$(:argv-err 1 'String name')}"
  local -n _us_gs_cl_str=${1:?}
: input "${_us_gs_cl_str:?$(:unset-err $1 'String value')}"
  local _us_gs_cl_prefc=${2:-"[ ]"}

  while :globmatch "$_us_gs_cl_prefc*" _us_gs_cl_str
  do
    #shellcheck disable=SC2295  # Match expansion from var is the idea here
    _us_gs_cl_str="${_us_gs_cl_str#$_us_gs_cl_prefc}"
    [[ "$_us_gs_cl_str" ]] || break
  done
}


if [[ ${0##*/} = common-dsl.bash ]]; then

  myArgsRead() {
    :read-args 'myArgsRead{A,B}' "$@"
    declare -p myArgsRead{A,B}
  }
  myArgsCopy() {
    :copy-args 'myArgsCopy{A,B}' "$@"
    declare -p myArgsCopy{A,B}
  }
  myArgsBind() {
    :bind-args 'myArgsBind{A,B}' "$@"
    declare -p myArgsBind{A,B}
  }

  exec {USER_FD}>&2
  trap 'exec {USER_FD}>&-' EXIT

  myArgsRead 123 abc
  myArgsCopy myArgsRead{A,B}
  myArgsBind myArgsCopy{A,B}

  declare -F :read-args
  declare -f :read-args
  declare -f myArgsRead
fi

# Id: common-dsl                          vim:set ft=bash sw=2 sts=2 et:
