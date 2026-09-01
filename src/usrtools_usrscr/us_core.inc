._() {
: name 'Core'
: about 'Initial step of bootstrap sequence'
#: type part  # TODO consolidate with older composure stuff:
: type group
: format part-function
# XXX: see old us-core.inc too:
: dynfun '.pass-status' 'return'
: dynfun '.set-status' 'return ${1:?Status int}'
: dynfun '.no-status' '"$@" || true'
: dynfun '.not-status' '! "$@"'
: nameals '.pass-status'   _IF   if-ok    :pass
: nameals '.set-status'    _STAT int-stat :stat
: nameals '.ignore-status' _     no-stat  :ignor
: nameals '.not-status'    _NOT  not-stat :not
}

._hooks() {
: -A
}
._hooks:global() {
: env "${_E_fail:=1}" 'Generic failure, catch-all for "fail"'
: env "${_E_bad:=2}"  'Usage error, same E:BSD:64, or E:MA + E:GAE'
: env "${_E_foo:=3}"  # _E_foo doesn't mean anything

: "${_E_nsc:=64}" # 64: nsc: no such command (id/group)
# 65: nss: no such source
# 66: nsf: no such function
: "${_E_nsk:=67}" # 67: nsk: no such key
: "${_E_nsa:=68}" # 68: nsa: no such action

: env "${US_BSD_SYSEXITS:='csv:
64,EX_USAGE,"Command-line usage error (bad arguments, bad flags, wrong syntax)"
65,EX_DATAERR,Input data was incorrect
66,EX_NOINPUT,Input file missing or unreadable
67,EX_NOUSER,User / addressee unknown
68,EX_NOHOST,Host unknown
69,EX_UNAVAILABLE,Service unavailable
70,EX_SOFTWARE,Internal software error
71,EX_OSERR,"Operating-system error (cannot fork, pipe, etc.)"
72,EX_OSFILE,Critical OS file missing
73,EX_CANTCREAT,Cannot create output file
74,EX_IOERR,Input/output error
75,EX_TEMPFAIL,Temporary failure – retry may help
76,EX_PROTOCOL,Remote protocol error
77,EX_NOPERM,Permission denied (higher-level)
78,EX_CONFIG,Configuration error'}"

# TODO: new range for different name space errors from 64-68
#: env "${_E_NSE:=}"        "Generice name space or lookup error"
#: env "${_E_NSSRC:=}"      "No such source name"
#: env "${_E_NSFUN:=}"      "No such function name"
#: env "${_E_NSKEY:=}"      "No such variable or key"
#: env "${_E_NSACT:=}"      "No such action or selector"
#: env "${_E_NSOPT:=}"      "No such command option"

: env "${_E_no_path:=120}"   # non-regular (dir, other) missing
: env "${_E_ifenv:=121}"     # incomplete or corrupt env, no measure specified or unsufficient
: env "${_E_doenv:=122}"     # incomplete or corrupt env, updated / partial success
: env "${_E_noenv:=123}"     # incomplete or corrupt env, no mitigation / final failure
: env "${_E_no_file:=124}"   # no-such-file(set): file missing or nullglob expansion encountered
: env "${_E_missing:=125}"   # missing, not implemented or to-do
: env "${_E_not_exec:=126}"  # NEXEC not-executable
: env "${_E_not_found:=127}" # NSFC no-such-file-or-command (generic: source or executable)
: env "${_E_status:=128}"    # Invalid argument to `exit` error state

  # 128+(1--64) is mapped for signals (see trap -l), ie. `kill -1 $PID` produces
  # exit status 129. Inserting Ctrl-C in a shell terminal produces exit 131.
  # On Linux last mapped status is 192: RTMAX (ie. 128+64).

: env "${_E_SIGHUP:=129}"    # 1: SIGHUP Hang-up exit
: env "${_E_SIGINT:=130}"    # 2: SIGINT (User) interrupt, kernel line-dicipline response to ctrl-c
: env "${_E_SIGILL:=132}"    # 4: SIGILL Illegal instruction exit
: env "${_E_SIGTRAP:=133}"   # 5: SIGTRAP Trace/breakpoint trap

: env "${_E_SIGKILL:=137}"   # 9: SIGKILL

: env "${_E_SIGTERM:=143}"   # 15: SIGTERM

: env "${_E_GAE:=193}"      "Generic argument error"
: env "${_E_MA:=194}"       "Arguments expected (missing-arguments) error"

# Some special states for "engines" controlling loops though nested handler functions,
: env "${_E_continue:=195}" "Unfinished, but step completed OK; can increment step until status change"
: env "${_E_next:=196}"     "Step was incomplete or next is wanted as alternative; must continue with next step"
: env "${_E_break:=197}"    "Success; last step, finish (batch) script or session"
: env "${_E_retry:=198}"    "Failed or throttled step (not E:continue, E:next, or E:fail), must reinvoke exactly"

# and thinking of windowed batch jobs and simple rule runners as well.
: env "${_E_limit:=199}"    "Other failure due to param OOB error or non-nominal signals, forced batch end"
: env "${_E_done:=200}"     "After batch/job set engine signals E:break and is finished and complete"
: env "${_E_more:=201}"     "After batch/job set engine signals E:break and is ready for reset/next window/pass"

: env "${ENV_CTX:=$$/${0##*/}}" "Set PID '/' basename for context indicator"
}

.clamp-nonzero-status() {
  ((_us_stat>255 && _us_stat=255 || _us_stat>-256 || _us_stat=1))
}

.clamp-status() {
  ((_us_stat==0)) ||
    _us_stat=$(( (_us_stat & 255) == 0 ? 1 : (_us_stat & 255) ))
}

.ignore-status() {
  "${@:?$FUNCNAME: Command invocation missing, $ENV_CTX}" || true
}

# XXX: this seems like something to generate or move to extra
.ignore-status-if() {
: input "${1:?$FUNCNAME: Ignore status spec expected}"
: input "${2:?$FUNCNAME: Wrapped command name missing}"
  local ignores=${1//[, ]/ } status ignore
  "${@:2}" && return
  status=$?
  for ignore in $ignores; do
    [[ $status -eq $ignore ]] && return
  done
  return $status
}

.not-status() {
  ! "${@:?$FUNCNAME: Command invocation missing, $ENV_CTX}"
}

.preserve-both() {
  declare _us_stat=$? _us_last=$_
}

.preserve-lastarg() {
  declare _us_last=$_
}

.preserve-status() {
  declare _us_stat=$?
}

.pass-both() {
  : "${_us_last?$FUNCNAME: Expected preserved last argument missing, $ENV_CTX}"
  return ${_us_stat:?$FUNCNAME: Expected preserved status missing, $ENV_CTX}
}

.pass-lastarg() {
  : "${_us_last?$FUNCNAME: Expected preserved last argument missing, $ENV_CTX}"
}

.pass-status() {
  return "${_us_stat:?$FUNCNAME: Expected preserved status missing, $ENV_CTX}"
}

.return-status() {
  return
}

.set-status() {
: param '~ <Status> ...'
  return ${1:?$FUNCNAME: Status integer, $ENV_CTX}
}

.set-status-clamped () {
: param '~ <Status> ...'
: about 'Clamp given <Status> integer to 0..255 value'
: extended 'In POSIX, status outside of 0..255 is undefined'
: extended 'Shells treat status as modulo 256; so 256 = 0'
: tag int util
  local _us_stat="${1:?$FUNCNAME: Status integer, $ENV_CTX}"

  \inline ...clamp-status
  return ${_us_stat}
}

.set-status-clamped-nz() {
: param '~ <Status> ...'
: about 'Clamp given <Status> integer to an actual, non-zero (1..255) value'
: extended 'See also .set-status-clamped() for a 0..255 clean up helper'
: tag int util
  local _us_stat="${1:?$FUNCNAME: Status integer, $ENV_CTX}"

  \inline ...clamp-nonzero-status
  return ${_us_stat}
}

# Id: usrtools_usrscr/us_core
