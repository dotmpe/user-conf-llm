._() {
: name 'Log'
: man '~ is a wrapper for the logger util.'
: type part
}
  
._hooks() {
: -A : ; }
._hooks:declare() {
: env "${LOG_LEVEL:=info}"
: env "${LOG_TAG:=${0##*/}}" "override per context"
: env "${LOG_FACILITY:=user}" "usually the only one open for loggger CLI"
: env "${LOG_TO_STDERR:=auto}" "always | never | auto/*"; }
._hooks:load() {
  case "${LOG_TO_STDERR}" in
  ( always ) LOGGER_STDERR=(-s) ;;
  ( never ) LOGGER_STDERR=() ;;
  ( auto|* ) if ((VERBOSITY>=0)); then
      LOGGER_STDERR=(-s)
    else
      LOGGER_STDERR=()
    fi
  esac; }


.log-do() {
: param '~ <Level> <Tag> <Message> [<context or param>]'
  local level_name=${1} base_tag=${2}
  # Join given tag base on LOG_TAG
  ${base_tag:1:1}

  logger -p "${LOG_FACILITY:?}.${level_name}" \
    -t "${LOG_TAG}" \
    "${LOGGER_STDERR[@]}" -- "${@:3}"
}

