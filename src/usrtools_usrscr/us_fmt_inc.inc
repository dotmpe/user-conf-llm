# .inc file, this is the reader and rewriter+indexer for primary shell source
# files src/*.inc in the User-Conf and User-Script projects.
#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
#
Include() {
: type group
}

._match-inblock-line() {
  local -n _from2=${1}
  case "${_from2}" in

  ( 'inline '* | '\inline; '* )
      : "${_from2##*___=}"
      : "${_%%;*}"
      inline_fun=${_}
      inliner=${_from2##*; }
      # say.err "Do inline _from2=${_from2@Q}"
      say.err "Have inliner=${inliner@Q} inline_fun=${inline_fun@Q}"
      # TODO
      return ${_E_retry:?}
    ;;

  ( ':- '* )
      : "${_from2//; /$';\n'}"
      : "${_//" && "/$' && \n'}"
      : "${_//" || "/$' || \n'}"
      local expanded=$_
      #...Pre-Processor._unshift-lineblock expanded
      ._unshift-lineblock expanded
      return ${_E_retry:?}
    ;;
  ( ':'[A-Za-z_-]* )
      : "${_from2% *}"
      us_pp_line=${us_pp_line/"${_}"/"_global ${_:1}"}
    ;;
  ( '.'[A-Za-z_-]* )
      : "${_from2% *}"
      us_pp_line=${us_pp_line/"${_}"/"${_ns0_pre}${_}"}
    ;;
  esac
}

.match-line() {
  local -n _from=${1}
  case "${_from}" in

  ( [A-Za-z_]*"() {"* )  :- : '  ._wrap-brace && ._wrap-line && ' ; ._read-modulename  ;;
  ( ": "* )              :- : '  ._wrap-brace && ._wrap-line && ' ; ._read-meta        ;;
  ( ":"[A-Za-z_-]* )     :- : '  ._wrap-brace && ._wrap-line && ' ; ._read-specialname ;;
  ( "."[A-Za-z_]*"() { : -A :"* )  :-  ._read-assoc ;;
  ( "."[A-Za-z_-]* )     :- : '  ._wrap-brace && ._wrap-line && ' ; ._read-localname   ;;

  ( \} )
      us_pp_line=${us_pp_line}" # /${_ns0_nameref}"
      if [[ ${_ns0_pre} != "${_ns0_nameref}" ]]; then
       _ns0_nameref=${_ns0_nameref%.*}
      fi
    ;;

  ( \#shellcheck\ * )
      return ${_E_next:?}
    ;;

  ( \#\ * | \# ) us_pp_line=
      return ${_E_continue:?}
    ;;

  ( \ * ) # Inside declarations match some indented lines as well
      if [[ ${_ns0_pre} != "${_ns0_nameref}" ]]; then
        local _scrline=${_from}
        :globstrip-charsleft _scrline $'[\ \t]'
        if [[ ${_scrline} = \#shellcheck\ * ]]; then
          return ${_E_next:?}
        elif [[ ${_scrline:0:1} = \# ]]; then
          us_pp_line=
          return ${_E_continue:?}
        fi
        ._wrap-line &&
        ._match-inblock-line _scrline || return
      fi
    ;;

  esac &&
  return ${_E_next:?}
}

._read-assoc() {
  local array_name=${us_pp_line%()*} function_block=
  declare -gA ${array_name:1}
  declare -n dest=$_
  while :; do
    ._read-line || failerr "While reading assoc" 1 || return
    case "${us_pp_line}" in
    ( "$array_name":[A-Za-z_]*"() {"* )
        : "${us_pp_line#"$array_name":}"
        : "${_%"() {"}"
        key=$_ us_pp_line=
      ;;
    ( [.A-Za-z_]*"() {"* ) break ;;
    ( \} ) dest[${key:?}]=${function_block:?} key= function_block= ;;
    ( \ * | :\ * ) function_block+=$us_pp_line$'\n' us_pp_line= ;;
    #( '' | \#* ) ;;
    #( * ) break ;;
    esac
  done
  _ns0_arr=$_ns0_pre.${!dest}
  ..Namespace.map-to-ns1 _ns{0,1}_arr &&
    # FIXME: should output as block
  :pass "$(
    ..Shell.dump-globals ${!dest}:${_ns1_arr:?}
  )" &&
  : "${_// \[/$'\n  ['}" &&
  : "${_//=\(\[/$'=(\n  ['}" &&
  printf '\n%s\n\n' "$_"
  [[ ! ${us_pp_line:+set} ]] || return ${_E_continue:?}
}

._read-localname() {
  us_pp_line=${_ns0_pre}${us_pp_line}
  _ns0_nameref=${us_pp_line%%"() {"*}
  us_pp_name_map["${_ns0_nameref}()"]=${us_pp_sources[-1]:?}:${us_pp_srclnr-}
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
  .map-to-ns1 _ns{0,1}_pre
  us_pp_block=${us_pp_line}"
: id ${_ns1_pre:?}
"
  us_pp_line=
  us_pp_name_map["${_ns0_pre}"]=${us_pp_sources[-1]:?}:${us_pp_srclnr-}
}

._read-specialname() {
  us_pp_line=${_ns0_pre}${us_pp_line}
  _ns0_nameref=${us_pp_line%%"() {"*}
  us_pp_name_map["${_ns0_nameref}()"]=${us_pp_sources[-1]:?}:${us_pp_srclnr-}
}

._wrap-brace() {
  if [[ ${us_pp_line: -1:1} == \} ]]; then
    us_pp_line=${us_pp_line/%\}}
    :globstrip-charsright us_pp_line $'[;\ \t]'
    us_pp_line+=$'\n'\}
    #...Pre-Processor._unshift-lineblock expanded
    ._unshift-lineblock us_pp_line
    return ${_E_retry:?}
  fi
}

._wrap-line() {
  if [[ ${us_pp_line} = *" :- "* ]]; then
    local expanded=${us_pp_line//' :- '/$'\n'}
    #...Pre-Processor._unshift-lineblock expanded
    ._unshift-lineblock expanded
    return ${_E_retry:?}
  fi
}

# Id: us-fmt-inc                                vim:set ft=bash sw=2 sts=2 et:
