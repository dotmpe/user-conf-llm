#!/usr/bin/env bash

\builtin . ./common_setup.bash

us-env -R us-env
declare -gA _os_script_{load,path}
us_part --hooks:declare,define,init us-term
: "${usp_opts:=--alias --hooks:declare,define,init --export}"

\builtin . ./common_env.bash
METADIR=.local

us_part $usp_opts --reload uc-loader
. <(uc_inc_pre usrtools_userconf/uc_docker)

# FIXME: uc_inc usrtools_userconf/uc_docker.inc
/usrtools_userconf/uc_docker/_hooks/declare
/usrtools_userconf/uc_docker/_hooks/init
if [[ ! ${uc_docker_imgconf_from['deb-bookworm:dev']:+set} ]]; then
  /usrtools_userconf/uc_docker/new_config \
    'deb-bookworm:dev' 'debian:bookworm-slim' \
    'bash ca-certificates curl git procps vim' \
    --extra 'RUN curl -s https://bashunit.com/install.sh | bash -s -- /usr/local/bin

WORKDIR /project
'
fi
if [[ ! ${uc_docker_imgconf_from['deb-bookworm-bash:dev']:+set} ]]; then

  # TODO: try buildpack-deps:bookworm to speed up build tools install

  /usrtools_userconf/uc_docker/new_config \
    'deb-bookworm-bash:dev' 'debian:bookworm-slim' \
    'build-essential wget libreadline-dev zlib1g-dev' \
    --extra '

ARG BASH_VERSION=5.2
RUN wget https://ftp.gnu.org/gnu/bash/bash-${BASH_VERSION}.tar.gz \
 && tar xzf bash-${BASH_VERSION}.tar.gz \
 && cd bash-${BASH_VERSION} \
 && ./configure --prefix=/usr \
 && make -j$(nproc) \
 && make install \
 && cd / && rm -rf /bash-${BASH_VERSION}*

WORKDIR /project
'
fi
# FIXME: cache_setmap should update session
/usrtools_userconf/uc_docker/_hooks/init
#declare -p uc_docker_imgconf_{from,packages,extra,settings}
#for img in "${!uc_docker_imgconf_from[@]}"; do
#  _ docker rmi "${img}"
#  /usrtools_userconf/uc_docker/build "$img"
#done
/usrtools_userconf/uc_docker/build 'deb-bookworm:dev'
/usrtools_userconf/uc_docker/start 'deb-bookworm:dev' "$@"
#
