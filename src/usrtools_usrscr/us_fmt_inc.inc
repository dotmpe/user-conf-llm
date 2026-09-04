# .inc file, this is the reader and rewriter+indexer for primary shell source
# files src/*.inc in the User-Conf and User-Script projects.
#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
#
Include() {
:
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

._match-line() {
  local -n _from=${1}
  case "${_from}" in

  ( [A-Za-z_]*"() {"* )  :-  ._read-modulename   ;;
  ( ": "* )              :-  ._read-meta         ;;
  ( ":"[A-Za-z_-]* )     :-  ._read-specialname  ;;
  ( "."[A-Za-z_-]* )     :-  ._read-localname    ;;

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
        if [[ ${_scrline} = *" :- "* ]]; then
          local expanded=${us_pp_line//' :- '/$'\n'}
          ._unshift-lineblock expanded
          return ${_E_retry:?}
        fi
        ._match-inblock-line _scrline || return
      fi
    ;;

  esac
  return ${_E_next:?}
}

# Id: us-fmt-inc                                vim:set ft=bash sw=2 sts=2 et:
