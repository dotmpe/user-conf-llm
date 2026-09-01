# .inc file, see User-Conf or User-Script us-inc for specification.
# 
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.

./_hooks/global() {
: env "${UC_ETC:=/etc/uc}"
: env "${UC_LIB:=${PREFIX:-/usr}/lib/uc}"
: env "${UC_SHARE:=${PREFIX:-/usr}/share/uc}"
}

./start_us.sh() {
: about 'Boot User-Scripts using locally deployed script parts'
: tag install profile
: prefix /usr/share/uc
: env BASH
: context ENV_SHELL

  if [ -s /usr/share/uc/start_us.bash ]; then
    if [ "${BASH-}" ] && [ "$BASH" != "/bin/sh" ]; then
      ENV_SHELL="bash"
    else
      ENV_SHELL="sh"
      say-warn "User-Scripts runs on full Bash only (skipping start_us)"
      return
    fi
    . /usr/share/uc/profile.bash ||
      say-err "User-Scripts status $? (trying to ignore)" || return 0
    [ -z "${env_build-}" ] &&
    eval "${US_ENV_INIT:?}" &&
    return || {
      [ ${env_build-} = 0 ] &&
      eval "${US_CORE_INIT:?}"
    }
  fi
}

./start_us.bash() {
: tag install
: prefix /usr/share/uc

  : "${USER:=$(whoami)}"
  : "${HOME:=/home/${USER}}"

  # Reference to this file, and to prepared caches
  : "${US_HOST_PROFILE_SHELL:=${UC_USER_LIB:=/usr/share/uc}/us-host-profile.$ENV_SHELL}"

  : "${US_HOST_PROFILE_CACHE:=${UC_DATA_DIR:=/var/lib/uc}/us-host-profile,cache.$ENV_SHELL}"
  : "${US_HOST_PROFILE_SHELL_CACHE:=${UC_DATA_DIR:=/var/lib/uc}/us-host-profile,cache.$ENV_SHELL}"
  : "${US_HOST_INTERACTIVE_SHELL_CACHE:=${UC_DATA_DIR:=/var/lib/uc}/us-host-interactive,cache.$ENV_SHELL}"

  #[[ ! ${USER:+set} ]] || {
  #: "${US_INTERACTIVE_SHELL_CACHE:=${UC_DATA_DIR:?}/us-interactive-$USER,cache.$ENV_SHELL}"
  : "${US_USER_PROFILE_SHELL_CACHE:=${UC_DATA_DIR:=/var/lib/uc}/$USER,profile,cache.$ENV_SHELL}"
  : "${US_USER_INTERACTIVE_SHELL_CACHE:=${UC_DATA_DIR:=/var/lib/uc}/$USER,interactive,cache.$ENV_SHELL}"
  #}
}

# Id: uc_profile                          vim:set ft=bash sw=2 sts=2 et:
