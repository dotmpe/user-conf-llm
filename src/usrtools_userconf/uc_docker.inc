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
: -A; }
./_hooks/declare() {
: "${UC_DOCKER_CACHE:=${CACHE_DIR:-.local/cache}}"
: "${UC_DOCKER_DATADIR:=${USER_DATA_DIR:-.local/user/data}}"
: "${UC_DOCKER_IMGCONF:=${UC_DOCKER_DATADIR:?}/uc_docker_imgconfig.data.bash}"
: "${UC_DOCKER_IMGCONF_MAPSPEC:=uc_docker_imgconf_{from,packages,extra}}"
  declare -ga uc_docker_imgconf_assocs
}
./_hooks/init() {
  \builtin read -ra uc_docker_imgconf_assocs < <(. <(echo echo ${UC_DOCKER_IMGCONF_MAPSPEC:?})) &&
  cache_loadmaps "${UC_DOCKER_CACHE:?}" "${uc_docker_imgconf_assocs[@]:?}"
}

./build() {
: param '~ <From-config> ...'
  (($#)) || return ${_E_MA:?}
: input "${1:?$FUNCNAME${*:+ ${*@Q}}: Configuration key expected}"
  local -n __imgconf_from='uc_docker_imgconf_from[$imgconf_key]'
  local -n __imgconf_packages='uc_docker_imgconf_packages[$imgconf_key]'
  local -n __imgconf_extra='uc_docker_imgconf_extra[$imgconf_key]'
  local imgconf_key=${1} dckr_file

  dckr_file=${UC_DOCKER_CACHE}/dockerfile-${1}.xyz
  local dockerfile_path="${dckr_file}"
  
  {
    echo "FROM ${__imgconf_from}"
    echo "RUN apt-get update && apt-get install -y ${!__imgconf_packages}"
    echo "COPY . /app"
    echo "WORKDIR /app"
    echo "CMD [\"/bin/bash\"]"
  } > "${dockerfile_path}"

  docker build -t "${imgconf_key}" -f "${dockerfile_path}" .
}

./new_config() {
: param '~ <Config-key> <From-image> [<packages> [options...]]'
: param '~ .. --packages <Names>'
: param '~ .. --extra <Additional script>'
  (($#)) || return ${_E_MA:?}
: input "${1:?$FUNCNAME${*:+ ${*@Q}}: Configuration key expected}"
: input "${2:?$FUNCNAME${*:+ ${*@Q}}: Base image reference expected}"
  local -n __imgconf_from='uc_docker_imgconf_from[$imgconf_key]'
  local -n __imgconf_packages='uc_docker_imgconf_packages[$imgconf_key]'
  local -n __imgconf_extra='uc_docker_imgconf_extra[$imgconf_key]'
  local imgconf_key=${1} imgconf_base=${2}

  shift 2 || return ${_E_GAE:?}
  cache_updatemap "${UC_DOCKER_CACHE:?}" "${!__imgconf_from}" "${imgconf_base}" ||
    failerr "E$? during save" || return
  while (($#)); do
  case "$1" in
    ( '' ) false ;;
    ( [!-]* )
        if [[ ! ${__imgconf_packages:+set} ]]; then
          cache_updatemap "${UC_DOCKER_CACHE:?}" "${!__imgconf_packages}" "${1}" ||
            return
          shift
        else false
        fi
      ;;
    ( --extra )
        cache_updatemap "${UC_DOCKER_CACHE:?}" "${!__imgconf_extra}" "${3}" ||
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
  TODO
}
# Id: uc_docker                          vim:set ft=bash sw=2 sts=2 et:
