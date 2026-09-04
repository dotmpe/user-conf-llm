# .inc file, see User-Conf or User-Script us-inc for specification.
#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
Terminal() {
: type group
}

._hooks() {
: -A
}

._hooks:global() {
  declare -gA us_term_map=()
  declare -gA \
  us_cterm_map=(
    # Console can do 16color iso. 8, if we tell it to
    [linux]=linux-16color
    # Afaik Xterm builds should be abe to do 256 (or at least 88)
    [xterm]=xterm-256color
  )

  # These are turned into terminal sequences by video-capenv
  declare -gA \
  us_term_capvar=(
    [BLINK]=blink
    [BOLD]=bold
    [NORMAL]=op
    [RESET]=sgr0
    [REVERSE]=rev
    [STANDOUT]=smso
    [STANDOUTOFF]=rmso
    [UNDERLINE]=smul
  )
}

# XXX: see also us_term_capvar... but that maps directly with tput
# would really like to just use tput names or capnames, not any new
# alias except for custom sequences
# see also man 5 terminfo, and infocmp -L
US_TERM_CURSOR='
PUT          \033[${1};${2}H         cup 2  # move to row {1} and column {2}
UP1          \033[A                  cuu1
DOWN1        \033[B                  cud1
UP           \033[${1}A              cuu 1
DOWN         \033[${1}B              cud 1
HOME         \033[H                  home
HIDECURSOR   \033[?25l               civis
CLEAR        \033[H\033[2J           clear
CLEARLINE    \r\033[K                cr+el
NORMCURSOR   \033[?12l\033[?25h      cnorm
BOTTOM       \033[$LINES;1H
BOTTOMCLEAR  \033[$LINES;1H\033[K
'

US_TERM_LINEDRAW='DRAW \033%\033(0 smacs
WRITE \033(B rmacs'

.cursor-position() {
: about 'Try and get cursor position or bail after exhausting retries'
: extended 'This seems to be more robust than other bare printf+read queries'
: extended 'Sleep was chosen rather arbitrarily, could depend on system load, emulator or host env a lot'
  local -I TERM_READ_{TIMEOUT,TRIES}
: input "${TERM_READ_TIMEOUT:=0.00001}"
: input "${TERM_READ_TRIES:=5}"
: input "${1:?$FUNCNAME${*:+ ${*@Q}}:Column variable}"
: input "${2:?$FUNCNAME${*:+ ${*@Q}}:Row variable}"
  local pos readfails=0
  local -n _ust_cp_col=${1} _ust_cp_row=${2}
  #local _stty=$(stty -g)
  #stty raw -echo
  # XXX: cannot make this work on urxvt
  #read -rsdR pos < <(printf '\e[6n' > /dev/tty)
  #stty "$_stty"
  printf '\e[6n'
  while ! read -rsdR pos
  do
    sleep ${TERM_READ_TIMEOUT} || continue
    ((readfails+=1))
    [[ $readfails -le $TERM_READ_TRIES ]] ||
      failerr "Could not get cursor position" || return
  done
  pos="${pos#*\[}"
  _ust_cp_row="${pos%;*}"
  _ust_cp_col="${pos#*;}"
}

.cursor-row() {
: input "${1:?$FUNCNAME${*:+ ${*@Q}}:Row variable}"
  .cursor-position _ "$1"
}

.define-codes() {
: about 'Define functions for codes from table'
: input "${*:?$FUNCNAME${*:+ ${*@Q}}:Terminal codemap name(s)}"

  local -n _codemap
  local scr
  for _codemap; do
    while read -r _call _code _
    do
      [[ ${_call-} ]] || continue
      # XXX: use alias?
      scr+=( "function ${_call} () { term_${_call,,} \"\$@\"; }" )
      scr+=( "function term_${_call,,} () { echo -en \"$_code\"; }" )
    done <<< "${_codemap:?$FUNCNAME:${!_codemap}: Terminal codemap value}"
  done &&
  . <(printf '%s\n' "${scr[@]}")
}

# It is possible to use tput initc to apply colors, ie. for linux console or
# OSC4 capable terminals. But the RGB would need to be scaled from the 2-byte
# 255 values to a 0-1000 decimal range (a spec that stems from 1980s Tektronix
# terminals). Virtually all modern terminals except iTerm support the OSC-4
# command anyway, and that can use regular 24-bit hexadecimals directly.

.load-system-palette() {
: about "Set 2x8 palette, foreground and background color from RGB"
: description "Load up to 19 swatch palette"
: extended "On linux, OSC P can set 8 colors (fg and bg are 7 and 0) normally"
: extended "Truecolor terminals can configure 16 colors through OSC 4, "
: extended "plus normal foreground, background and cursor with OSC 10-12."
: completion 'complete -A variable'
  local -n _palette=${1:?$FUNCNAME: Palette table variable, $ENV_CTX}
  local i=0 r g b rest
  while read -r r g b rest
  do
    [[ $TERM == linux* ]] && {
      printf "\e]P%01X%s%s%s\a" $i $r $g $b
      #[[ $i -lt 7 ]] || break
      [[ $i -lt 15 ]] || break
    } || {
      [[ $i -lt 16 ]] && printf "\x1b]4;%d;rgb:%s/%s/%s\a" $i $r $g $b
      [[ $i -eq 16 ]] && printf "\x1b]10;rgb:%s/%s/%s\a" $r $g $b
      [[ $i -eq 17 ]] && printf "\x1b]11;rgb:%s/%s/%s\a" $r $g $b && break
      # TOTEST: not sure if cursor rgb is 21? (and \e]12 is indexed)
      [[ $i -eq 18 ]] && printf "\x1b]12;rgb:%s/%s/%s\a" $r $g $b && break
    }
    ((i+=1))
  done <<< "${_palette:?$FUNCNAME:$1: Palette table value, $ENV_CTX}"
}

.load-truecolor-vars() {
: about "Define any number of colors from RGB for truecolor terminals"
: completion 'complete -A variable'
  local -n _palette=${1:?$FUNCNAME: Palette table variable, $ENV_CTX}
  local _r _g _b _name{,s}
  while read -r _r _g _b _names
  do
    for _name in $_names
    do
      printf -v $_name    "\x1b[38;2;%d;%d;%dm" 0x$_r 0X$_g 0X$_b
      printf -v BG_$_name "\x1b[48;2;%d;%d;%dm" 0x$_r 0X$_g 0X$_b
    done
  done <<< "${_palette:?$FUNCNAME:$1: Palette table value, $ENV_CTX}"
}

._read-flags() {
  : "${NO_READLINE:-0}"
  if ! ((NO_READLINE)); then
    : "${BASH_FOR_READLINE_COMPLETION:+E}"
    : "${_:-e}"
    read_flags+=$_
  fi

  : "${READ_ECHO:-1}"
  ((_)) || read_flags+=s

  : "${READ_METACHARS:-0}"
  ((_)) || read_flags+=r
}

._read-options() {
  if [[ ${READ_INITVALUE:+set} ]]; then
    read_opts+=( -i "$READ_INITVALUE" )
  fi

  if [[ ${READ_NCHARS:+set} ]]; then
    : "${READ_UNTIL_DELIMITER:-1}"
    if [[ ${READ_RECORD_DELIMITER:+set} ]] && ((_)); then
      read_opts+=( -n "$READ_NCHARS" )
    else
      read_opts+=( -N "$READ_NCHARS" )
    fi
  fi

  if [[ ${READ_PS:+set} ]]; then
    read_opts+=( -p "$READ_PS" )
  fi

  if [[ ${READ_RECORD_DELIMITER:+set} ]]; then
    read_opts+=( -d "$READ_RECORD_DELIMITER" )
  fi
}

.read-input() {
: param '~ [options] [<Var> <Read-args...>]'
: tag util
  local read_{flags,opts}
  . <(sh_funbody ${FUNCNAME%.*}._read-flags ) &&
  if [[ ${read_flags:+set} ]]; then
    read_opts=( -"$read_flags" )
  fi &&
  . <(sh_funbody ${FUNCNAME%.*}._read-options) &&
  read -r "${read_opts[@]}" "${@:-read_input}" #</dev/tty
}

.user-input() {
: param '~ [options] [<Var> <Read-args...>]'
  local -n _us_term_varref1
  local a boolean=0 bit=0 offset=0

  for (( a = 1; a <= $#; a++ ))
  do case ${!a} in
    ( --t-ish | --trueish ) boolean=1 offset=$#
        local READ_NCHARS=1 READ_RECORD_DELIMITER="" ;;
    ( --bitish | --trueish-bit ) boolean=1 bit=1 offset=$#
        local READ_NCHARS=1 READ_RECORD_DELIMITER="" ;;
    ( --char )
        local READ_NCHARS=1 ;;

    ( [A-Za-z_]* ) _us_term_varref1=${!a}; break ;;
    ( -* ) continue ;;
    ( [!A-Za-z_-]* )
      failerr "Invalid argument for $FUNCNAME: ${1@Q}" || return ;;
    #( * ) failerr "Unexpected match value ${1@Q}" || return
  esac; done &&
  if [[ ! ${!_us_term_varref1:+set} ]]; then
    failerr "Variable name expected" || return
  fi &&
  if ((offset)); then
    .read-input "${@:1:offset-2}" "${@:offset}" || return
  else
    .read-input "${@}" || return
  fi &&

  if [[ ! ${_us_term_varref1:+set} ]]; then
    _us_term_varref1=${READ_DEFAULT-}
  fi &&

  if ((boolean)); then
    ! ((bit)) || local US_STR_TRUE=1 US_STR_FALSE=0
    ..String.map-normalize --boolean ${!_us_term_varref1} || return
  fi
}

.video-capenv() {
  local var fail=0
  local -n capname='us_term_capvar[$var]'
  for var in "${!us_term_capvar[@]}"
  do
    : "${capname:?$FUNCNAME${*:+ $*}: Capability name for $var}"
    ..Core.pass-status "$(2>/dev/null tput $capname)" &&
    declare -g $var=$_ || {
      failerr "No $capname for terminal $TERM"
      fail=1
      declare -g $var
    }
  done
  ! ((fail)) || return
}

# XXX: cleanup move checks to init routes?
.video-colorenv() {
  # XXX: testing dynamic check here. Would be nice to keep for user facing
  # commands. Or to not mess up any terminal really.
  { ! tput initc >& /dev/null && {
      # Only needed for xterm
      printf "\033]4;?\007" &&
      read -rs -d $'\007' -t 0.01 response &&  # Read until BEL, timeout 0.01s
      ! [[ $response =~ ^$'\033]4;rgb:' ]]
    }
    #[[ $TERM != xterm-256color ]]
    # XXX: tmux doesnt support either tput initc or any OSC queries, look at
    # passthrough config.
    # gnome-terminal reports xterm-256color and should support initc
    # but for some reason doesnt get here (or do any us-env-init?)

  } || {
    # Load custom system colors (ie. 2x8 palette)
    .load-system-palette "${1}"

    # NOTE: background will not be redrawn until a console switch or clear,
    # below command will force redraw now using setterm. It is however
    # fairly interruptive as it blanks for long time. Better to just switch
    # console or clear by hand to properly apply palette if desired. Do
    # not want to clear on init (ie. after login while MOTD and messages
    # show) normally.
    #setterm --blank force
    #setterm --blank poke
  }

    # Define variables to set foreground and background for each palette color
    # as well.
    .load-truecolor-vars "${1}" ||
      failerr "E$? configuring terminal for palette $1" || return

  { tput -S <<end
setaf
setab
end
  } >& /dev/null && {

    local i
    # Map terminal palette to {f,b}{0..15} for string template use
    for ((i=0;i<16;i++))
    do
      read -r _f$i <<< "$(tput setaf $i)"
      read -r _b$i <<< "$(tput setab $i)"
    done

  } || {

    { ! tput -S <<end
setf
setb
end
    } >& /dev/null || {

      local i b
      # Map to names and {f,b}{0..15} for string template use
      for ((i=0;i<16;i++))
      do
        read -r _f$i <<< "$(tput setf $i)"
        read -r _b$i <<< "$(tput setb $i)"
      done
    }
  }
}

# Id: us_term                          vim:set ft=bash sw=2 sts=2 et:
