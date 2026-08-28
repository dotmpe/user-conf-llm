Say() {
: about 'Echo helper to message the user'
: TODO fix man indexing
  _man='Echo helper to message the user.

Support for VERBOSITY level and VERBOSE or QUIET bit settings.

## Use case: user surface output messaging.

User options:

- Default: human-oriented output. Rule of silence: do not print "everything OK"
  messages.

- Verbose: more often and more in detail output. Show the user something at least
  that is achieved by an invocation.

- Quiet: suppress informational output. The opposite of verbose, tell the user
  only what is *not* right.

- Silent: let the status speak for itself.

Scripts or commands should decide on how exactly to use above schemes, depending
on their own options and needs.

'
: type part
}

._hooks() { : -A : ; }
._hooks:declare() {
: env "${ENV_CTX:=$0[$$]}"
: env "${VERBOSITY:=1}"
: env "${US_CHANNELS:=STD ERR DIAG USER DEBUG}"
}

.say-when() {
: param '~ <Level> <Message> ...'
: tag util
: input "${1?$(:argv-err 1 'Verbosity level')}"
: input "${2?$(:argv-err 2 'Verbal message')}"
  local level=${1:-0} message=${2}
  ((level>3 && level=3 || level>-1 || level=3))

  if ((VERBOSITY>=level)); then
    TODO
  fi
}

.say-with-status() {
  local stat=${2:-$?}
: about 'Output message and pass-trough/override non-zero status'
: extended "Status value is clipped to 0>...>256, cannot be 0"
: als failerr
: tag util
: input "${1?$(:argv-err 1 'Failure message')}"

  ((stat>255 && stat=255 || stat>0 || stat=1))
  inline-part +colorterm .say_stderr
  # '{ >&2 echo "$1" }'
  return ${stat}
}

# XXX: this is extra stuff for user conf/script. Just pick a base palette swatch
# or two, use a map, something like that for us-say defaults.
.say-with-status+colorterm() {
: about 'Part for .say-with-status to run if color is enabled, supported and loaded'
: tag part
: inline-if COLOR COLORTERM NORMAL

  # XXX: do per-status customized color? But would like better
  # script capabilities setup first, and may just defer to customizable log.sh
  if [[ $stat -eq 0 ]]; then
      >&2 echo "${C_PASS-}$1${NORMAL-}"
  elif [[ $stat == 1 ]]; then
      >&2 echo "${C_ABNORMAL-}$1${NORMAL-}"
  elif [[ $stat == 2 ]]; then
      >&2 echo "${C_ERROR-}$1${NORMAL-}"
  elif [[ $stat -gt 2 && $stat -lt 128 ]]; then
      >&2 echo "${C_ABNORMAL-}$1${NORMAL-}"
  elif [[ $stat -gt 127 && $stat -lt 193 ]]; then
    # space used for some signal triggered return/exit numbers
      >&2 echo "${C_AUXILIARY-}$1${NORMAL-}"
  elif [[ $stat -gt 192 && $stat -lt 255 ]]; then
    # unused space
      >&2 echo "${C_CONTEXT-}$1${NORMAL-}"
  else
    # overflow?
      >&2 echo "${C_SECONDARY-}$1${NORMAL-}"
  fi
}

.write-at() {
: param '~ <FD> <Message> ...'
: input "${1?$(:argv-err 1 'Reserved FD_ tag expected')}" "See US_CHANNELS list"
: input "${2?$(:argv-err 2 'Message expected')}"
  local -n __FD=FD_${1}

  >&$__FD echo "$1"
}

# Id: us_say                                     vim:set ft=bash sw=2 sts=2 et:
