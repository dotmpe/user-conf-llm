# .inc file, see User-Conf or User-Script us-inc for specification.
#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
Shell() {
: type group
}

.byname-concat-after() {
: param '~ <Variable-name> <Separator> <Value> ...'
: src us-util.bash
: id sh-byname-concat-after
: completion 'complete -A variable'
: input "${1:?$(:argv-err 1 'Variable name')}"
: input "${2:?$(:argv-err 2 'Separator')}"
: input "${3:?$(:argv-err 3 'Append value')}"
  local -n _var=${1}
  _var=${_var-}${_var:+${2}}${3}
}

.byname-set-value() {
: src us-util.bash
: id sh-byname-set
: completion 'complete -A variable'
: input "${1:?$(:argv-err 1 'Variable name')}"
: input "${2?$(:argv-err 2 'Variable value')}"
  local -n _var=${1}
  _var=${3}
}

.command-not-found-handle() {
: about 'Implementation for Bash command_not_found_handle'
  local rs \
    idname=${1//[^A-Za-z0-9_]/_} \
    sidname=${1//[^A-Za-z0-9-]/-}
  if >/dev/null 2>&1 declare -F $idname; then rs="function $idname"; fi
  # FIXME: if [[ -f "${C_INC:?}/$sidname.inc" ]]; then rs="${rs:+$rs, and }$sidname.inc"; fi
  if [[ -n ${rs-} ]]
  then >&2 printf "%s: command not found (but found %s)\n" "$1" "$rs"
    return 127
  fi

: XXX "This is default bash.bashrc on Debian, but only if c-n-f is installed."
  if [[ -x /usr/lib/command-not-found ]]
  then /usr/lib/command-not-found -- "$1"
    return $?
  elif [[ -x /usr/share/command-not-found/command-not-found ]]
  then
    /usr/share/command-not-found/command-not-found -- "$1"
    return $?
  else >&2 printf "%s: command not found (and suggestions unavailable)\n" "$1"
    return 127
  fi
}

.dump-push() {
: input "${*:?$(:argv-err \* 'Variable name(s)')}"
  local var
  for var
  do
    :pass "$(declare -p $var)" &&
    : "${_#"declare -"[aA]" $var="}" &&
    echo -e "$var+=(\n  ${_:1: -1}\n)"
  done | sed -E '
    s/'"'"'\ \[/'"'"'\n  [/g
    s/"\ \[/"\n  [/g
  '
}

.dump-globals() {
: about 'Helper to make variable dumps global for re-use within functions'
: param '~ <Vars...>'
: completion 'complete -A variable -A arrayvar'
: input "${*:?$(:argv-err \* 'Variable name(s)')}"
  local var als
  for var; do
    case "$var" in ( *:* ) als=${var#*:} var=${var%:*};; ( * ) als=; esac
    :pass "$(declare -p $var)" &&
    : "${_#declare -}" &&
    : "${_#-}" &&
    : "${_%% *} ${als:-$var}=${_#*=}" &&
    echo "declare -g${_}"
  done
}

.expand-static() {
: about "Perform expansion for shell string"
: input "${1:?$(:argv-err 1 'Shell string')}"
: input "${2:?$(:argv-err 2 'Array variable name')}"
: completion 'complete -A arrayvar'
  #shellcheck disable=2162   # read without -r
  :pass "$(\builtin eval "echo $1")" &&
  <<< "${_}" read -a ${2:?}
}

.function-body() {
: param '<Ref-fun> [<Dest-var>] ...'
: aliases sh-fbody fun-body
: input "${1:?$(:argv-err 1 'Function name expected')}"
  (($#-1)) &&
  local -n _out=${2:?$(:argv-err 2 'Output name expected')} ||
    local _out
  :pass "$(typeset -f "$1")" || return
  : "${_#* () }"
  : "${_:4:-2}"
  _out="$_"
  (($#>1)) || echo "$_out"
}

.function-eval() {
: param '<Function-name> <Function-body>'
: input "${1:?$(:argv-err 1 'Function name')}"
: input "${2:?$(:argv-err 2 'Function body')}"
  : "${*:2}"
  : "${1} () { ${_:?}; }"
  \builtin eval "$_"
}

.function-exists() {
: param ' ~ <Funcname> ...'
: completion 'complete -A function'
: input "${1:?$(:argv-err 1 'Function name')}"
  :pass "$(declare -F -- "${_}")"
}

.maptabvars-defaults() {
: param ' ~ <Table-string-var> ...'
: completion 'complete -A variable'
: input "${1:?$(:argv-err 1 'Table name')}"
  local -n _tab=${1}
: input "${_tab:?$(:unset-err $1 "Table value ")}"
  local dest sources
  while read -r dest sources; do
    declare -n _dest=$dest _src
    [[ ! ${_dest:+set} ]] || continue
    for _src in $sources
    do [[ ${_src:+set} ]] || continue
      _dest=$_src
      continue 2
    done
    :failerr "$FUNCNAME: No value for ${!_dest} (from $sources, ignored)"
  done <<< "${_tab}"
}

.mapvars-defaults() {
: param ' ~ <Array-var> ...'
: completion 'complete -A arrayvar'
: input "${1:?$(:argv-err 1 'Variable name')}"
  local -n _map=${1}
#: input "${_map:?$(:unset-err $1 "Variable value")}"
  local o=0
  while [[ $o -lt ${#_map[@]} ]]; do
    # XXX: to test
    declare -n _dest='_map[@]:o:1' _src='_map[@]: o+1:1'
    #declare -n _dest=${_map[@]:o:1} _src=${_map[@]: o+1:1}
    [[ ${_dest:+set} ]] || {
      [[ ${_src:+set} ]] &&
      _dest=$_src ||
      :ignore :failerr "$FUNCNAME: No value for ${1} (from ${2}, ignored)"
    }
    ((o+=2))
  done
}

.meta-for() {
: param ':0 ~ <To-var> <Field-key> ...'
: completion 'complete -A variable'
: input "${1:?$(:argv-err 1 'Destination variable')}"
: input "${2:?$(:argv-err 2 'Field name')}"
  local _line
  while read -r _line; do
    [[ ${_line} =~ ^[$' \t']*([:\ _]+)${2}\ +[\'\"]?([^\'\"\;]+) ]] || continue
    break
  done
  [[ ${BASH_REMATCH[2]:+set} ]] || return
  #shellcheck disable=2059  # using variable in printf format
  printf -v "${1}" "${BASH_REMATCH[2]}"
}

.reference() {
: about 'Print first found reference for symbol(s)'
: param ' ~ <Symbols...>'
: completion 'complete -avc -A function'
  local _441d30cd_ref _441d30cd_all=0
  case "${1}" in
    ( --all ) _441d30cd_all=1; shift
  esac
  for _441d30cd_ref
  do {
    # TODO: find old routines again, weave that together and make this a bit smarter
      # Try print anything relevant, return or continue on first success
      .Symbol.alias-main --show "${_441d30cd_ref}" ||
      declare -p "${_441d30cd_ref}" ||
      declare -f "${_441d30cd_ref}" ||
      help "${_441d30cd_ref}" ||
      apropos "${_441d30cd_ref}" ||
      man "${_441d30cd_ref}"
    } 2>/dev/null && {
      ((_441d30cd_all)) || return 0
    } ||
      stderr "No refence found for '${_441d30cd_ref}'"
    # Recurse for known aliased names
    [[ ! ${us_interactive_aliasmap["$_441d30cd_ref"]:+set} ]] ||
      "$FUNCNAME" --all "${us_interactive_aliasmap["$_441d30cd_ref"]}"
  done
}

.Symbol.alias-main() {
: TODO 'integrate with user command(s)'
  case "${1:?}" in
  ( --match-all )
      local aliases
      ..System.read-call aliases compgen -A alias -X "!${2:?}" &&
      .Symbol.alias-main "${@:3}" "${aliases[@]}"
    ;;
  ( --command )
      shift
      local als stat
      for als
      do
        :pass "$(type "${als}")" &&
        : "${_#"$als is aliased to \`"}" &&
        : "${_%\'}" &&
        echo "$_" || stat=$?
      done
      return ${stat-}
    ;;
  ( --expand )
      shift
      local cmd def scr stat newscr
      while [[ ${cmd:+set} || ${#} -gt 0 ]]
      do
        [[ ${cmd:+set} ]] || {
          cmd=${1:?$(:argv-err 1 'Alias symbol')}
          shift
        }
        def="$(type "${cmd}")" &&
        >&2 echo declare def=$def &&
        [[ $def == ${cmd}" is aliased to "* ]] && {
          : "${def#"$cmd is aliased to \`"}" &&
          : "${_%\'}" &&
          newscr=$_ &&
          : "${newscr%% *}" &&
          test "$newscr" != "${_}" &&
          cmd=$_ scr=${newscr}${scr:+ $scr} || {
            echo "${newscr}${scr:+ $scr}"
            unset scr cmd
          }
        } || {
          echo "${cmd}${scr:+ $scr}"
          unset cmd scr
        }
      done
    ;;
  ( --show )
      local aliases
      ..System.read-call aliases compgen -A alias -X "!${2:?}" &&
      ..System.call-apply type aliases
  esac
}

.references() {
: about 'Print all references for symbol(s)'
: param ' ~ <Symbols...>'
: completion 'complete -avc -A function'
  .reference --all "$@"
}

.reload-env() {
: input "${1:?$(:argv-err 1 'Function name')}"
: completion 'complete -A function'
  local src
  :pass "$(declare -F "${1}")" &&
  read -r _ _ src <<< "${_}" &&
  . "${src:?}"
}

.reload-source() {
: input "${1:?$(:argv-err 1 'Function name')}"
  TODO "reload $* propery (see .reload-env); try grouping sources for cache files to do this"
}

.try-unalias() {
: about 'Quickest way to erase an alias'
: ext 'EAFP; checking for alias under Bash involves either type (bad worst-case performance), or caching either compgen or alias -p output actually.'
: completion 'complete -A alias'
  2>/dev/null unalias "$@" || true
}

.variable-type() {
: about '~ <Symbols...>'
: about 'Cache for just the declare flags'
# XXX: not using ${var@A} bc that abbreviates non-export globals so not sure if
# that is better/faster
  local sym
  local -n _sh_vfl='us_shell_tspec["$sym"]'
  for sym; do
    if [[ ! ${_sh_vfl:+set} ]]; then
      :pass "$(2>/dev/null declare -p "${sym}")" ||
        :failerr "E$? getting ${sym@Q} typeset" || return
      : "${_:8}"
      : "${_%% *}"
      _sh_vfl=$_
    fi
  done
}

# Id: us_shell                          vim:set ft=bash sw=2 sts=2 et:
