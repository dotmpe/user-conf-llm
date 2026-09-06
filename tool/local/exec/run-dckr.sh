#!/bin/bash

# XXX: (bbb) harness testrunning docker image config

scr_pre=tool/local
\builtin . $scr_pre/common_script.bash
\builtin . $scr_pre/common-dsl.bash

#us_part $usp_opts --reload uc-loader
#. <(uc_inc_pre usrtools_usrconf/uc_dckr)
# FIXME: uc_inc usrtools_usrconf/uc_dckr.inc
. src/usrtools_usrscr/us_dckr.inc

._hooks:global
._hooks:load

if [[ ! ${uc_docker_imgconf_from['deb-bookworm:dev']:+set} ]]; then
  .new_config \
    'deb-bookworm:dev' 'debian:bookworm-slim' \
    'bash ca-certificates curl git procps vim' \
    --extra \
'RUN curl -s https://bashunit.com/install.sh | bash -s -- /usr/local/bin
COPY etc/bash/user-rc.sh /root/.bashrc
COPY etc/profile.d/usrtools_usrscr_host_profile.sh /etc/profile.d/
COPY pack/ns1 /usr/share
WORKDIR /project
'
fi

if [[ ! ${uc_docker_imgconf_from['deb-bookworm-bash:dev']:+set} ]]; then

  # TODO: try buildpack-deps:bookworm to speed up build tools install

  .new_config \
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
._hooks:load

.build 'deb-bookworm:dev'

#declare -p uc_docker_imgconf_{from,packages,extra,settings}
#for img in "${!uc_docker_imgconf_from[@]}"; do
#  _ docker rmi "${img}"
#  /usrtools_usrconf/uc_docker/build "$img"
#done
.start 'deb-bookworm:dev' "$@"
#
