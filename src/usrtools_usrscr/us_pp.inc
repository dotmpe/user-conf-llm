#.inc file, see User-Conf or User-Script us-inc for specification.
# 
# Copyright 2026 .mpe  <me@dotmpe.com>
# 
# Distributed under terms of the MIT license.
#

Pre-Processor() {
: tag group
}
._hooks() { : -A : ; }
._hooks:global() {
: env "${US_PP_CACHE:=${CACHE_DIR:-.local/cache}}"
: env "${US_PP_DATADIR:=${USER_DATA_DIR:-.local/user/data}}"
: env "${US_PP_STATE:=${US_PP_DATADIR:?}/us_pp_state.data.bash}"
#: env "${US_PP_STATE_MAPSPEC:=uc_pp_{name_map,meta_static\}}"
#  declare -ga us_pp_state_assocs
}
._hooks:load() {
: env "${_E_break:?}"
: env "${_E_continue:?}"
: env "${_E_MA:?}"
: env "${_E_next:?}"
: env "${_E_retry:?}"
  #\builtin read -ra us_pp_state_assocs < <(
  #  \builtin . <(echo echo ${US_PP_STATE_MAPSPEC:?})) &&
  #cache_loadmaps "${US_PP_STATE:?}" "${us_pp_state_assocs[@]:?}"
  cache_loadmaps "${US_PP_STATE:?}" us_pp_{name_map,meta_static}
}

._concat-source () {
  local new_fd
  ._open-filesource "$1" &&
  # NOTE read is at end of stack, so concat is actually unshift
  us_pp_input_stack=( "$new_fd" "${us_pp_input_stack[@]}" )
  us_pp_input_lnr=( 0 "${us_pp_input_lnr[@]}" )
}

._match-inblock-line() {
  local -n _from2=${1}
  case "${_from2}" in

  ( :_\ * )
      : "${_from2//; /$';\n'}"
      : "${_//" && "/$' && \n'}"
      : "${_//" || "/$' || \n'}"
      local expanded=$_
      ._unshift-lineblock expanded
      return ${_E_retry:?}
    ;;
  ( ":"[A-Za-z_-]* )
      : "${_from2% *}"
      us_pp_line=${us_pp_line/"${_}"/"_global ${_:1}"}
    ;;
  ( "."[A-Za-z_-]* )
      : "${_from2% *}"
      us_pp_line=${us_pp_line/"${_}"/"${_ns0_pre}${_}"}
    ;;
  esac
}

._match-line() {
  local -n _from=${1}
  case "${_from}" in

  ( [A-Za-z_]*"() {"* )  :_  ._read-modulename   ;;
  ( ": "* )              :_  ._read-meta         ;;
  ( ":"[A-Za-z_-]* )     :_  ._read-specialname  ;;
  ( "."[A-Za-z_-]* )     :_  ._read-localname    ;;

  ( \} )
      us_pp_line=${us_pp_line}" # /${_ns0_nameref}"
      if [[ ${_ns0_pre} != "${_ns0_nameref}" ]]; then
       _ns0_nameref=${_ns0_nameref%.*} 
      fi
    ;;

  ( \#shellcheck\ * )
      return ${_E_next:?}
    ;;

  ( \#\ * | \# ) : no op
      return ${_E_retry:?}
    ;;

  ( \ * ) 
      if [[ ${_ns0_pre} != "${_ns0_nameref}" ]]; then
        local _scrline=${_from}
        :globstrip-charsleft _scrline $'[\ \t]'
        if [[ ${_scrline} = \#shellcheck\ * ]]; then
          return ${_E_next:?}
        elif [[ ${_scrline:0:1} = \# ]]; then
          return ${_E_retry:?}
        fi
        if [[ ${_scrline} = *" :_ "* ]]; then
          local expanded=${us_pp_line//' :_ '/$'\n'}
          ._unshift-lineblock expanded
          return ${_E_retry:?}
        fi
        ._match-inblock-line _scrline || return
      fi
    ;;

  esac
  return ${_E_next:?}
}

._open-filesource () {
  exec {new_fd}<"$1" ||
    failerr "E$? opening FD for file ${1@Q}" || return
  us_pp_sources+=( "$1" )
  :_debug "Reading from ${1@Q} (FD #${new_fd})"
}

._read-line() {
  (( ${#us_pp_input_stack[@]} == 0 )) && return ${_E_break:?}
  ._shift-line || return
  ((us_pp_srclnr+=1))
  # XXX: optional: strip trailing CR, count lines, etc.
  return 0 
}

._read-localname() {
  us_pp_line=${_ns0_pre}${us_pp_line}
  _ns0_nameref=${us_pp_line%%"() {"*}
  us_pp_name_map["${_ns0_nameref}()"]=${us_pp_input}:${us_pp_srclnr-}
}

._read-meta() {
  local _{meta,value} _metaline=${us_pp_line}
  :globstrip-charsleft _metaline $'[:\ \t]'
  read -r _{meta,value} <<< "${_metaline}"
  local -n _dest="us_pp_meta_static[\"$_ns0_nameref : ${_meta:?}\"]"
  [[ ! ${_value:+set} ]] || _dest=${_dest:+"$_dest"$'\n'}${_value:?}
}

._read-modulename() {
  us_pp_line=${_ns0_pre}.${us_pp_line}
  _ns0_nameref=${us_pp_line%%"() {"*}
  say.v "Found module ${_ns0_nameref}"
  _ns0_pre=${_ns0_nameref}
  us_pp_name_map["${_ns0_pre}"]=${us_pp_input}:${us_pp_srclnr-}
}

._read-specialname() {
  us_pp_line=${_ns0_pre}${us_pp_line}
  _ns0_nameref=${us_pp_line%%"() {"*}
  us_pp_name_map["${_ns0_nameref}()"]=${us_pp_input}:${us_pp_srclnr-}
}

._shift-line() {
  if ! IFS= read -r us_pp_line <&$us_pp_fd; then
    # EOF on current FD
    exec {us_pp_fd}<&- || {
      us_pp_fail_stat=$?
      us_pp_fail_fd=$us_pp_fd
      failerr "closing FD $us_pp_fd" "$us_pp_fail_stat" || return
    }
    unset us_pp_fd
    (( ${#us_pp_input_stack[@]} == 0 )) && return ${_E_break:?}
    return ${_E_next:?}
  fi
}

._unshift-lineblock() {
  local new_fd
  local -n _str=${1}
  [[ ${_str:+set} ]] ||
    failerr "No block to unshift for ${FUNCNAME[1]}: ${1@Q}" || return
  exec {new_fd}<<<"$_str" ||
    failerr "E$? opening FD for string ${1}" || return
  us_pp_input_stack+=( "$new_fd" )
  us_pp_input_lnr+=( 0 )
  us_pp_sources+=( "<string:$new_fd>" )
  :_debug "Reading lineblock [${#_str}] from ${FUNCNAME[1]} at FD #${new_fd}"
}

._unshift-source () {
  local new_fd
  ._open-filesource "$1" &&
  # NOTE read is at end of stack, so unshift is just push
  us_pp_input_stack+=( "$new_fd" )
  us_pp_input_lnr+=( 0 )
}

.run() {
: param '~ <Source-path> ...'
: tag util
: env _E_MA
  (($#)) || return ${_E_MA:?}
: input "${1:?$(:argv-err 1 "Source path")}"
  local us_pp_input=${1}
  ___=._run-common+startup; inline-fun
  
  while true; do

    if [[ -z ${us_pp_line:+set} || ${us_pp_retry:-0} -eq 1 ]]; then
      ._read-line || {
        local st=$?
        (( st == _E_break )) && break
        (( st == _E_next  )) && continue
        failerr "At $us_pp_lnr:$us_pp_srclnr while trying to read $us_pp_input" $st || return
      }
      ((us_pp_lnr+=1))
    fi

    ._match-line us_pp_line && handled=1 || {
      local st=$?
      (( st == _E_continue )) && continue
      (( st == _E_retry    )) && { us_pp_line= ; continue; }
      (( st == _E_next     )) && handled=0 ||
        failerr "At $us_pp_lnr:$us_pp_srclnr ${us_pp_line@Q}" $st || return
    }

    if (( !handled )); then
      case ${us_pp_mode:-normal} in
      ( normal )  ._emit-line ;;  # pass-through / rewrite
      ( block )   ._block-append ;;
      ( * )
        failerr "unknown mode ${us_pp_mode@Q}" || return
      esac
    fi

    us_pp_line=
  done

  if ((DEBUG)); then
    # Peek at results
    # TODO: sort tables at output
    to-v printf.lines.array-map.tab us_pp_name_map
    to-v printf.lines.array-map.tab us_pp_meta_static
  fi

  # TODO: manage state per source
  #cache_dumpmaps "${US_PP_STATE:?}" "${us_pp_state_assocs[@]}"
  >| "${US_PP_STATE:?}" sh_dumpglobal us_pp_{name_map,meta_static}
}

._emit-line() {
  printf.line "${us_pp_line}"
}

._block-append() {
  current_block+=${us_pp_line}$'\n'
}

._run-common+startup() {
  # FIXME: give global access to accumulated sources, for cache invalidation etc.
  # Should probably prepare a copy for these on request, after wrapping up run
  declare -a us_pp_sources
  #declare -A us_pp_name_map
  #declare -A us_pp_meta_static

  local us_pp_{line,mode,retry}

  local current_block

  local -n us_pp_fd='us_pp_input_stack[-1]'
  local -n us_pp_srclnr='us_pp_input_lnr[-1]'
  local -a us_pp_input_{stack,lnr}
  local us_pp_lnr=0

  [[ ${us_pp_lang-} ]] ||
    _ failerr "Language expected for prefilter and/or comment + shebang/modeline formats (ignored)"
  : "${us_pp_lang:=}"

  # First apply static script as filter on input
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

  #if ((do_outline))
  #then
  #  declare -ga us_pp_outline{,_tags}
  #  us_pp_outline_level=0
  #fi

  # TODO: plugin namespace translator
  local _ns0_{pre,namref}
  _ns0_pre=User-Script
  _ns0_nameref=${_ns0_pre}
}

# Id: us_pp                                      vim:set ft=bash sw=2 sts=2 et:
