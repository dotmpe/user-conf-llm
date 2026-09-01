# .inc file, see User-Conf or User-Script us-inc for specification.
# 
# Copyright 2026 .mpe  <me@dotmpe.com>
# 
# Distributed under terms of the MIT license.
Trace() {
: type group
}

.callers() {
: param '~ [<Frame>]'
: copy "sys.lib.sh"
  local i
  for (( i=${1:-0}; 1; i++ ))
  do caller $i || break
  done
}

.callers-fmt() {
: param '~ <Format> <Frame-> <idx...>'
: copy "sys.lib.sh"
  local fmt=${1:?} frame=${2:-0} varg i=0 j ; local -a map
  for varg in "${@:3}"
  do
    map[i]=$varg
    ((i+=1))
  done &&
  [[ ${map[*]:+set} ]] &&
  for j in "${!map[@]}"
  do
    local -n _field$j=_in${map[j]}
  done &&
  local _in{0,1,2} &&
  for (( i=${frame}; 1; i++ ))
  do
    : "$(caller $i)" &&
    test -n "$_" &&
    IFS=$'\n ' read -r _in{0,1,2} <<< "$_" &&
    printf "$fmt" "$_field0" "$_field1" "$_field2" ||
      break
  done
}

.callstack-str() {
  local -n _cs_str=${1:?}
  local i
  for ((i=${#FUNCNAME[@]}-1;i>0;i--))
  do
    _cs_str=${_cs_str:+$_cs_str:}${FUNCNAME[i]}
  done
  ! (($#-1)) && return
  local argstr
  printf -v argstr -- '%s:' "+$(( $# - 1 ))" "${@:1}"
  _cs_str=${_cs_str:+$_cs_str:}$argstr
}

.format() {
  local -n __us_trace_fmt=UC_TRACE_FMT \
    _g1l=US_TRACE_GLYPH_OPENLEFT \
    _g1r=US_TRACE_GLYPH_OPENRIGHT \
    _g2l=US_TRACE_GLYPH_OUTLEFT \
    _g2r=US_TRACE_GLYPH_OUTRIGHT
}

.shell-error-exit() {
: param '~ <stat> <msg> <frame-offset> <dest> ...'
  local -n __errstat=$1 __out_str=${4:?} \
    _g1l=US_TRACE_GLYPH_OPENLEFT \
    _g1r=US_TRACE_GLYPH_OPENRIGHT \
    _g2l=US_TRACE_GLYPH_OUTLEFT \
    _g2r=US_TRACE_GLYPH_OUTRIGHT \
    _c1=US_TRACE_PADD_1 \
    _c2=US_TRACE_PADD_2 \
    _c3=US_TRACE_PADD_3 \
    _c4=US_TRACE_PADD_4 \
    _c5=US_TRACE_PADD_5 \
    _c6=US_TRACE_PADD_6
  [[ ${_c1:+set} ]] || .update-columns

  local n=${NORMAL-} b=${BOLD-} r=${REVERSE-} R=${RESET-}

  # Assemble output from header piece followed by formatted trace
  local _header_label
  [[ "${2-}" ]] && _header_label="$2" || _header_label=$BASH_COMMAND

  printf -v c1p "%-${_c1}s" ''
  printf -v c3p "%-${_c3}s" ''

  local _header
  printf -v _header " ${_f1-}${_g1l}${r}${c1p}${R}${_f1-}${BASH_UC_SCRIPTNAME:+ $BASH_UC_SCRIPTNAME} ${b}error:${n} command %${_c2}s %s" \
    "${b}${BASH_UC_SCRIPTTAG:+$BASH_UC_SCRIPTTAG }${n}${_header_label@Q}" \
    "${R}${_f6-}returned status ${_f11-}${b}E${R}${_f11-}$__errstat${R}:" ||
    false

  local _trace_str
  # FIXME: fix real stack
  #.format ${3:-2} _trace_str
  # Using simpler caller impl for source context description
  # choose between simple default caller output and formatted list for clarity
  # This
  #_trace_str=$(.callers "${3-}")
  _trace_str=$(.callers-fmt \
"  ${c3p}${b}${_f15-}%-${_c4}s ${r}${_f7-}${_g2r}${_g2r}${_g2r} at ${R} ${_f12-}%${_c5}i ${r}${_f7-}${b} in ${R} ${_f2-}%-${_c6}q ${r}${_f7-} ${_g2l}${R}\n" \
"${3:-2}" 1 0 2) || true

  # FIXME: report only if inherited (set -E)
  [[ ${_trace_str} ]] || return
  __out_str="$_header${_trace_str:+$'\n'$_trace_str}"
}

.shell-print-error-exit() {
  local _err=$? _out x=0 y
  # TOTEST: ! ((INTERACTIVE))
  [[ ${PS1:+set} ]] && {
    User-Script.Terminal.cursor-position x y
  }
  .shell-error-exit _err "" "" _out && {
    ((x == 0 )) && : "" || : $'\n'
  } && echo "$_$_out"
  return $_err
}

# FIXME: make sys-callers compatible but from variables. Add fields based on
# options
# TODO: rename sh_src_ctxstr
#sh_src_context () # source/line context as normally reported for undef exception
.callcontext-str() {
  .sourcecontext-str 2
}

.sourcecontext-str() {
: id sh-src-context/common,us
  local i=${1:-1}
  echo "${BASH_SOURCE[i]}:${BASH_LINENO[i-1]}"
}

.update-columns() {
# NOTE: COLUMNS should be set by shell normally
  local -I COLUMNS
  : "${COLUMNS:=79}"
  # Spacing makes large text much more legible but requires tracking of
  # metachars used in the template by hand.
  US_TRACE_PADD_1=$(( COLUMNS / 12 ))
  US_TRACE_PADD_2=$(( 3 * US_TRACE_PADD_1 ))
  US_TRACE_PADD_3=$(( US_TRACE_PADD_1 / 2 ))
  US_TRACE_PADD_4=$(( 5 * US_TRACE_PADD_1 ))
  US_TRACE_PADD_5=$(( US_TRACE_PADD_1 / 2 ))
  US_TRACE_PADD_6=$(( 4 * US_TRACE_PADD_1 ))
  US_TRACE_COLS=$COLUMNS
}

# Id: us_trace                          vim:set ft=bash sw=2 sts=2 et:
