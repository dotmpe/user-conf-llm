#.inc file, see User-Conf or User-Script us-inc for specification.
#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
#
Pre-Processor() {
: tag group
: man '~ is a Bash read loop that manages a source stack and invokes a line
matcher. If unhandled by the matcher, the pre-processor loop emits the line
again.

The matcher'\''s return status determines the behavior of the loop.

  0              Read next line
  _E_break       Stop read loop
  _E_continue    Continue with current line (but keep value)
  _E_next        Emit and read next line
  _E_retry       Re-read line value (but do not increment line counter)

Both E:continue and E:retry skip the default emit but keep invoking the matcher.
But only E:continue keeps the line data until it clears. These two can be used
to reprocess data. While us_pp_line can hold one line of preprocessed data,
multiple lines of data must be pushed back onto the source stack as a block, so
that an actual line can be re-read from it. '
}
._hooks() { : -A : ; }
._hooks:global() {
: env "${US_PP_DATADIR:=${USER_DATA_DIR:-.local/user/data}}"
: env "${US_PP_STATE:=${US_PP_DATADIR:?}/us_pp_state.data.bash}"
  declare -g us_pp_run_settings
}
._hooks:load() {
: env "${_E_break:?}"
: env "${_E_continue:?}"
: env "${_E_MA:?}"
: env "${_E_next:?}"
: env "${_E_retry:?}"
  
: env "${US_PP_RUN_SPEC:=us_pp_{input:._source,reader:cb,matcher:cb\}}"
  \builtin read -ra us_pp_run_settings < <(
    \builtin . <(echo echo ${US_PP_RUN_SPEC:?}))

  :cache-loadmaps "${US_PP_STATE:?}" us_pp_{name_map,meta_static}
}

._block-append() {
  us_pp_block+=${us_pp_line}$'\n'
}

._concat-source () {
  local new_fd
  ._open-filesource "$1" &&
  # NOTE read is at end of stack, so concat is actually unshift
  us_pp_input_stack=( "$new_fd" "${us_pp_input_stack[@]}" )
  us_pp_input_lnr=( 0 "${us_pp_input_lnr[@]}" )
}

._open-filesource () {
  exec {new_fd}<"$1" ||
    :failerr "E$? opening FD for file ${1@Q}" || return
  us_pp_sources+=( "$1" )
  :_debug "Reading from ${1@Q} (FD #${new_fd})"
}

._read-line() {
  (( ${#us_pp_input_stack[@]} == 0 )) && return ${_E_break:?}
    # TODO: pass var?
  ._shift-line || return
  ((${2:-0})) || ((us_pp_srclnr+=1))
  # XXX: optional: strip trailing CR, count lines, etc.
  return 0
}

._shift-line() {
  if ! IFS= read -r us_pp_line <&${us_pp_fd:?}; then
    # close after EOF on current FD
    exec {us_pp_fd}<&- || {
      us_pp_fail_stat=$?
      us_pp_fail_fd=$us_pp_fd
      :failerr "closing FD $us_pp_fd" "$us_pp_fail_stat" || return
    }
    unset us_pp_fd
    (( ${#us_pp_input_stack[@]} == 0 )) && return ${_E_break:?}
    return ${_E_next:?}
  fi
}

._source() {
: about 'Reset .run to given source file'
  us_pp_input=${1}
  local new_fd
  if [[ ! ${us_pp_filter-} ]]
  then
    exec {new_fd}<"$us_pp_input"
  else
    exec {new_fd}< <(< "$us_pp_input" . <(echo "${us_pp_filter}"))
  fi
  us_pp_sources+=( "$us_pp_input" )
  us_pp_input_stack=( "$new_fd" )
  us_pp_input_lnr=( 0 )
}

._unshift-lineblock() {
  local new_fd
  local -n _str=${1}
  [[ ${_str:+set} ]] ||
    :failerr "No block to unshift for ${FUNCNAME[1]}: ${1@Q}" || return
  exec {new_fd}<<<"$_str" ||
    :failerr "E$? opening FD for string ${1}" || return
  us_pp_input_stack+=( "$new_fd" )
  us_pp_input_lnr+=( 0 )
  us_pp_sources+=( "<string:$new_fd>" )
  :_debug "Reading lineblock [${#_str}] from ${FUNCNAME[1]} at FD #${new_fd}"
}

._unshift-source() {
  local new_fd
  ._open-filesource "$1" &&
  # NOTE read is at end of stack, so unshift is just push
  us_pp_input_stack+=( "$new_fd" )
  us_pp_input_lnr+=( 0 )
}

.run() {
: param '~ <Source> [<Matcher>] ...'
: tag util
: input "${1:?$(:argv-err 1 "Source path")}"

  #:read-setting US_PP_RUN_SPEC "$@"
  local us_pp_matcher=${2:-User-Script.Format.Include.match-line}
  #\builtin . <(:funbody ._run-common+startup)
  inline ___=._run-common+startup; inline-fun
  ._source "$1" || return

  while ((1)); do

    if [[ -z ${us_pp_line:+set} || ${us_pp_retry:-0} -eq 1 ]]; then
      ._read-line us_pp_line ${us_pp_line:+1} || {
        local st=$?
        (( st == _E_break )) && break
        (( st == _E_next  )) && continue
        : "$us_pp_lnr"; (( _ == us_pp_srclnr )) || : "$_:$us_pp_srclnr"
        :failerr "At $_ while trying to read $us_pp_input" $st || return
      }
      ((${us_pp_retry:-0})) && us_pp_retry=0 || ((us_pp_lnr+=1))
    fi

    "$us_pp_matcher" us_pp_line && handled=1 || {
      local st=$?
      (( st == _E_break    )) && break
      #shellcheck disable=2154
      (( st == _E_continue )) && continue  # ASSERT !us_pp_line should be consumed
      #shellcheck disable=2154
      (( st == _E_retry    )) && { us_pp_retry=1; continue; }
      (( st == _E_next     )) && handled=0 || {
        : "$us_pp_lnr"; (( _ == us_pp_srclnr )) || : "$_:$us_pp_srclnr"
        :failerr "At $_ ${us_pp_line@Q}" $st || return
      }
    }

    if (( !handled )); then
      if [[ ${us_pp_block:+set} ]]; then
        printf '%s' "$us_pp_block"
        us_pp_block=
      else
        printf '%s\n' "$us_pp_line"
        us_pp_line=
      fi
    fi
  done

  if (( ${#us_pp_input_stack[*]} > 0 )); then
    say.err "Closing ${#us_pp_input_stack[*]} remaining unread inputs"
    local fd
    for fd in "${us_pp_input_stack[@]}"; do
      exec {fd}<&- || say.err "E$? closing FD $fd (ignored)"
    done
  fi

  if ((DEBUG)); then
    # Peek at results
    # TODO: sort tables at output
    :to-v printf.lines.array-map.tab us_pp_name_map
    :to-v printf.lines.array-map.tab us_pp_meta_static
  fi

  # TODO: manage state per source
  #cache_dumpmaps "${US_PP_STATE:?}" "${us_pp_state_assocs[@]}"

  >| "${US_PP_STATE:?}" \
  ..Shell.dump-globals us_pp_{name_map,meta_static}
  #:dump-pretty-globals us_pp_{name_map,meta_static}
}

._run-common+startup() {
  local us_pp_{block,line,mode,retry}

  local -n us_pp_fd='us_pp_input_stack[-1]'
  local -n us_pp_srclnr='us_pp_input_lnr[-1]'
  local -a us_pp_input_{stack,lnr}
  local us_pp_lnr=0

  [[ ${us_pp_lang-} ]] ||
  ! ((DEBUG)) ||
    say.err "Language expected for prefilter and/or comment + shebang/modeline formats (ignored)"
  : "${us_pp_lang:=}"

  # FIXME: give global access to accumulated sources, for cache invalidation etc.
  # Should probably prepare a copy for these on request, after wrapping up run
  declare -a us_pp_sources
  #declare -A us_pp_name_map
  #declare -A us_pp_meta_static

  #if ((do_outline))
  #then
  #  declare -ga us_pp_outline{,_tags}
  #  us_pp_outline_level=0
  #fi

  local _ns0_{pre,nameref}
  # TODO: map other way
  _ns0_pre=User-Script
  .map-to-ns1 _ns{0,1}_pre
  _ns0_nameref=${_ns0_pre}
}

# Id: us_pp                                      vim:set ft=bash sw=2 sts=2 et:
