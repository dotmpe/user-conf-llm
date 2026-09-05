# .inc file, see User-Conf or User-Script us-inc for specification.
#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
Dev() {
: tag dev
}
._hooks() { : -A : ; }
._hooks:global() {
# XXX: example layout for data module with hashtables sequence in cache
: env "${US_DEV_CACHE:=${CACHE_DIR:-.local/cache}}"
: env "${US_DEV_DATADIR:=${USER_DATA_DIR:-.local/user/data}}"
: env "${US_DEV_STATE:=${US_DEV_DATADIR:?}/uc_dev_state.data.bash}"
: env "${US_DEV_STATE_MAPSPEC:=uc_dev_{map{0..3\}\}}"
  declare -ga us_dev_state_assocs
: env "${UC_DEV_PATHS:=${UC_DEV_PATHS:-.}}"
  User-Script.Operating-System.path-append "${C_INC:?}" "${US_BIN:?}" "${U_S:?}" "${U_C:?}" "${UCONF:?}" UC_DEV_PATHS
}
._hooks:load() {
  \builtin read -ra us_dev_state_assocs < <(
    \builtin . <(echo echo ${US_DEV_STATE_MAPSPEC:?}))
  ..Cache.load-maps "${US_DEV_STATE:?}" "${us_dev_state_assocs[@]:?}"
}

.scan() {
: about 'Scan for TODO/XXX/FIXME tags in project directories'
  ! (($#)) || return ${_E_GAE:?}
: tag util
: env "${UC_DEV_PATHS:=${UC_DEV_PATHS:-.}}"
  local path args

  for path in ${UC_DEV_PATHS//:/ }; do
    if [[ -d ${path} ]]; then
      args=( -r -E '^\ *# .*\<TODO|XXX|FIXME' )
      args+=( --binary-files=without-match )
      args+=( --exclude-dir "node_modules" )
      args+=( --exclude-dir ".meta" )
      args+=( --exclude-dir ".local" )
      args+=( --exclude-dir ".git" )
      args+=( --exclude-dir ".venv" )
      if [[ -e ${path}/.gitignore ]]; then
        #while read -r glob; do
        #  [[ ! ${glob:+set} || $glob = \#* ]] && continue
        #  #if [[ $glob = *\** ]]; then
        #  if [[ $glob = */* ]]; then
        #    args+=( --exclude-dir "$glob" )
        #  else
        #    args+=( --exclude "$glob" )
        #  fi
        #done <"$path/.gitignore"
        #printf.lines.array args
        :
      fi
      echo "Scanning ${path} for tags..."
      \builtin command grep "${args[@]}" "${path}" || true
    else
      say.err "Warning: ${path} is not a directory (ignored)"
    fi
  done
}
