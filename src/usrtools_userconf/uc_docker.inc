# .inc file, see User-Conf or User-Script us-inc for specification.
# 
# Copyright 2026 .mpe  <me@dotmpe.com>
# 
# Distributed under terms of the MIT license.
./() {
: tag cli util group
: env "${_E_GAE:=193}" "${_E_MA:=194}"
}
./_hooks() {
: FIXME : -A
}
./_hooks/declare() {
: env "${UC_DOCKER_CACHE:=${CACHE_DIR:-.local/cache}}"
: env "${UC_DOCKER_DATADIR:=${USER_DATA_DIR:-.local/user/data}}"
: env "${UC_DOCKER_IMGCONF:=${UC_DOCKER_DATADIR:?}/uc_docker_imgconfig.data.bash}"
: env "${UC_DOCKER_IMGCONF_MAPSPEC:=uc_docker_imgconf_{from,packages,extra,settings\}}"
  declare -ga uc_docker_imgconf_assocs
}
./_hooks/init() {
  \builtin read -ra uc_docker_imgconf_assocs < <(
    \builtin . <(echo echo ${UC_DOCKER_IMGCONF_MAPSPEC:?})) &&
  cache_loadmaps "${UC_DOCKER_IMGCONF:?}" "${uc_docker_imgconf_assocs[@]:?}"
}

./build() {
: about 'Build docker image from configuration'
: param '~ <From-config> ...'
  (($#)) || return ${_E_MA:?}
: input "${1:?$FUNCNAME${*:+ ${*@Q}}: Image-config key expected}"
  local -n __imgconf_from='uc_docker_imgconf_from[$imgconf_key]'
  local -n __imgconf_packages='uc_docker_imgconf_packages[$imgconf_key]'
  local -n __imgconf_extra='uc_docker_imgconf_extra[$imgconf_key]'
  local -n __imgconf_settings='uc_docker_imgconf_settings[$imgconf_key]'
  local imgconf_key=${1} dckrfile_path

  dckrfile_path=${UC_DOCKER_CACHE}/uc_docker_imgconf-${1//[^A-Za-z0-9_]/_}.dockerfile
  [[ -w ${dckrfile_path%/*} ]] ||
    failerr "Target directory not writable: ${dckrfile_path@Q}" || return

  # TODO: Parse any extension settings
  
  {
    echo "FROM ${__imgconf_from}"
    if [[ ${__imgconf_packages:+set} ]]; then
      echo "RUN apt-get update && \\
        apt-get install -y --no-install-recommends \\
        ${__imgconf_packages} && \\
        rm -rf /var/lib/apt/lists/*"
    fi
    #if [[ ${__imgconf_appcopy:+set} ]]; then
    #  echo "COPY . /app"
    #  echo "WORKDIR /app"
    #fi
    if [[ ${__imgconf_extra:+set} ]]; then
      echo "${__imgconf_extra:?}"
    fi
    #if [[ ${__imgconf_entrypoint:+set} ]]; then
    #  echo "ENTRYPOINT ${__imgconf_entrypoint:?}"
    #fi
    #if [[ ${__imgconf_defcmd:+set} ]]; then
    #  echo "CMD ${__imgconf_defcmd:?}"
    #fi
  } > "${dckrfile_path}" &&

  \builtin command docker build -t "${imgconf_key}" -f "${dckrfile_path}" .
}

./new_config() {
: about 'Create new docker image configuration'
: param '~ <Config-key> <From-image> [<packages> [options...]]'
: param '~ .. --packages <Names>'
: param '~ .. --extra <Additional script>'
  (($#)) || return ${_E_MA:?}
: input "${1:?$FUNCNAME${*:+ ${*@Q}}: Image-config key expected}"
: input "${2:?$FUNCNAME${*:+ ${*@Q}}: Base image reference expected}"
  local -n __imgconf_from='uc_docker_imgconf_from[$imgconf_key]'
  local -n __imgconf_packages='uc_docker_imgconf_packages[$imgconf_key]'
  local -n __imgconf_extra='uc_docker_imgconf_extra[$imgconf_key]'
  local imgconf_key=${1} imgconf_base=${2}

  shift 2 || return ${_E_GAE:?}
  [[ ! ${__imgconf_from:+set} ]] ||
    failerr "Configuration ${imgconf_key@Q} already exists" || return
  cache_updatemap "${UC_DOCKER_IMGCONF:?}" "${!__imgconf_from}" "${imgconf_base}" ||
    failerr "E$? during save" || return
  while (($#)); do
  case "$1" in
    ( '' ) false ;;
    ( [!-]* )
        if [[ ! ${__imgconf_packages:+set} ]]; then
          cache_updatemap "${UC_DOCKER_IMGCONF:?}" "${!__imgconf_packages}" "${1}" ||
            return
          shift
        else false
        fi
      ;;
    ( --extra )
        [[ ${2:+set} ]] || failerr "Empty argument after ${1@Q}" || return
        cache_updatemap "${UC_DOCKER_IMGCONF:?}" "${!__imgconf_extra}" "${2}" ||
          return
        shift 2 || return ${_E_GAE:?}
      ;;
    ( * ) false
  esac || failerr "${1@Q}? Option or argument error" ${_E_GAE:?} || return; done
}

./update_config() {
: param '~ <Config-key> <options and params for config...>'
  TODO
}

./start() {
: about 'Start temporary, new container from image and attach'
: param '~ <Config-key> <options and params for container or special (entrypoint)...>'
  (($#)) || return ${_E_MA:?}
: input "${1:?$FUNCNAME${*:+ ${*@Q}}: Image-config key expected}"
  local imgconf_key=${1}

  \builtin command docker run \
    -v $PWD:/project:rw \
    --workdir /project \
    --rm -it "${imgconf_key}" "${@:2}" 
}

# Id: uc_docker                          vim:set ft=bash sw=2 sts=2 et:
