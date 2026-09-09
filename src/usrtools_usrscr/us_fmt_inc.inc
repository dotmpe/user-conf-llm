## .inc file, this is the reader and rewriter+indexer for primary shell source
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
      #...Pre-Processor.unshift-lineblock expanded
      .unshift-lineblock expanded
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
: tag preproc hook
  local -n _from=${1}
  case "${_from}" in

  ( [A-Za-z_][A-Za-z0-9_.+-]*"() {"* )  :- : '  ._wrap-brace && ._wrap-line && ' ; ._read-modulename  ;;
  ( ": "[A-Za-z_]* )                    :- : '  ._wrap-brace && ._wrap-line && ' ; ._read-meta        ;;
  ( ":"[A-Za-z_][A-Za-z0-9_.+-]* )      :- : '  ._wrap-brace && ._wrap-line && ' ; ._read-specialname ;;
  ( "."[A-Za-z_][A-Za-z0-9_./:+-]*"() { : -A :"* )  :-  ._read-assoc ;;
  ( "."[A-Za-z_][A-Za-z0-9_./:+-]* )    :- : '  ._wrap-brace && ._wrap-line && ' ; ._read-localname   ;;

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
  # Output assoc with value(s) from following functions (the body script), and
  # key from the name. The initial function is to declare the base name.
  local array_name=${us_pp_line%()*} function_block=
  declare -gA ${array_name:1}
  declare -n dest=$_
  while ((1)); do
    # XXX: should probably try to cleanup state a bit, but this may just be
    # _E_continue (etc) too.
    .read-line || return
    case "${us_pp_line}" in
    ( "$array_name":[A-Za-z_][A-Za-z0-9_.:+-]*"() {"* )
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
  # Get the package prefixed name, and typeset the var+value
  _ns0_arr=$_ns0_pre.${!dest}
  ..Namespace.map-to-ns1 _ns{0,1}_arr &&
  :pass "$(:dump-pretty-globals ${!dest}:${_ns1_arr:?})" &&
  # And emit the definition as block
  us_pp_block=${_:?}$'\n'${us_pp_line:+$'\n'$us_pp_line$'\n'} us_pp_line= &&
  return ${_E_continue:?}
}

._read-localname() {
  us_pp_line=${_ns0_pre}${us_pp_line}
  _ns0_nameref=${us_pp_line%%"() {"*}
  us_pp_name_map["${_ns0_nameref}()"]=${us_pp_sources[-1]:?}:${us_pp_srclnr-}
}

._read-meta() {
  local _{curval,meta,value} _metaline=${us_pp_line}
  :globstrip-charsleft _metaline $'[:\ \t]'
  read -r _{meta,value} <<< "${_metaline}"

  local -n _dest="us_pp_meta_static[\"$_ns0_nameref : ${_meta:?}\"]"

  _value=${_value%[\"\']}
  _value=${_value#[\"\']}
  if [[ ${_value:+set} ]]; then
    [[ ! ${_dest:+set} ]] || while read -r _curval; do
      [[ ${_value} = "${_curval}" ]] && return
    done <<< "$_dest"
    _dest=${_dest:+"$_dest"$'\n'}${_value:?}
  fi
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
    #...Pre-Processor.unshift-lineblock expanded
    .unshift-lineblock us_pp_line
    return ${_E_retry:?}
  fi
}

._wrap-line() {
  if [[ ${us_pp_line} = *" :- "* ]]; then
    local expanded=${us_pp_line//' :- '/$'\n'}
    #...Pre-Processor_unshift-lineblock expanded
    .unshift-lineblock expanded
    return ${_E_retry:?}
  fi
}

# Id: us-fmt-inc                                vim:set ft=bash sw=2 sts=2 et:
