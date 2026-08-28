# Bootstrap from User-Script parts

# FIXME: this file is mostly prototype setup; should check here for either
# profile or interactive cache copy(s) and simply load that. Same for profile
# or bashrc but the problem is both have a user specific part.

[ "${BASH-}" ] && [ "$BASH" != "/bin/sh" ] &&
ENV_SHELL="bash" || ENV_SHELL="sh"

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

# XXX: should really always only build from interactive login, but need to build
# for every user at least. Then depending on complexity of setup, other build
# matrices are possibly required.
[[ ${PS1-} ]] && {
  caches=( "$US_HOST_PROFILE_SHELL_CACHE" "$US_HOST_INTERACTIVE_SHELL_CACHE"
    "$US_USER_PROFILE_SHELL_CACHE" "$US_USER_INTERACTIVE_SHELL_CACHE" )
} ||
  caches=( "$US_HOST_PROFILE_SHELL_CACHE" "$US_USER_PROFILE_SHELL_CACHE" )

: "${ENV_DEV:=false}"
: "${ENV_DEBUG:=0}"
: "${UC_DEBUG:=0}"
#: "${US_DEBUG:=0}"
: "${US_PROFILE_BASE_SEQ:=us-core us-std us-str us-os us-debug us-part}"
: "${US_PROFILE_CORE_SEQ:=us-palette us-arr us-sys us-shell us-log us-profile-env us-profile-log us-trace us-term}"

! ((ENV_DEBUG)) &&
#! (($US_DEBUG)) &&
! ((UC_DEBUG)) ||
  PS4='${BASH_SOURCE:+\[\033[34m\]\$BASH_SOURCE\[\033[36m\]:\[\033[32m\]\${LINENO}} \[\033[33m\]+\[\033[0m\] '

! ((UC_DEBUG)) || set -x

export US_CORE_INIT=": \"\${usp_opts:=--alias --hooks:declare,define,init}\"
[[ \${ENV_DEV-} != true ]] || usp_opts=\$usp_opts\ --reload\ --reinit

[[ \${PS1-} ]] && {
  . \"$US_HOST_PROFILE_SHELL_CACHE\" &&
  us_part --init \$usp_opts \${US_ENV_PARTS//[:,]/ } &&
  . \"$US_HOST_INTERACTIVE_SHELL_CACHE\" ||
    >&2 echo \"E\$? Interactive shell cache load failure\"
} || {
  . \"$US_HOST_PROFILE_SHELL_CACHE\" &&
  us_part --init \$usp_opts \${US_ENV_PARTS//[:,]/ } ||
    >&2 echo \"E\$? Non-interactive shell cache load failure\"
}"

export US_ENV_PARTS=${US_PROFILE_BASE_SEQ}\ ${US_PROFILE_CORE_SEQ}
export US_ENV_INIT=": \"\${usp_opts:=--alias --hooks:declare,define,init}\"
[[ \${ENV_DEV-} != true ]] || usp_opts=\$usp_opts\ --reload\ --reinit

[[ \${PS1-} ]] && {
  . \"$US_HOST_PROFILE_SHELL_CACHE\" &&
  us_part --init \$usp_opts \${US_ENV_PARTS//[:,]/ } &&
  . \"$US_HOST_INTERACTIVE_SHELL_CACHE\" &&
  . \"$US_USER_PROFILE_SHELL_CACHE\" &&
  . \"$US_USER_INTERACTIVE_SHELL_CACHE\" ||
    >&2 echo \"E\$? Interactive user shell cache load failure\"
} || {
  . \"$US_HOST_PROFILE_SHELL_CACHE\" &&
  us_part --init \$usp_opts \${US_ENV_PARTS//[:,]/ } &&
  . \"$US_USER_PROFILE_SHELL_CACHE\" ||
    >&2 echo \"E\$? Non-interactive user shell cache load failure\"
}"

# XXX: no dev modes, if caches exist return else build

[[ ! ${PS1-} ]] && {
  [[ -s "${caches[0]}" ]] && {
    [[ -s "${caches[1]}" ]] && return || env_build=0 user_build=1
  } || env_build=1
} || {
  [[ -s "${caches[0]}" && -s "${caches[1]}" ]] && {
    [[ -s "${caches[2]}" && -s "${caches[3]}" ]] && return || env_build=0 user_build=1
  } || env_build=1
}

if [ -n "${USER-}" ] && [ -s /etc/uc/user/${USER-} ]
then
  . /etc/uc/user/$USER
  echo . /etc/uc/user/$USER >> "$US_USER_PROFILE_SHELL_CACHE"
fi

! ((env_build)) && return


# TODO: rename --alias, maybe to --dsl:local and prefix all als/ssc dsn's
usp_opts=--alias\ --hooks:declare,define,init
#case " ${ENV_OPT-} " in (*" autoexport "*)
    usp_opts=$usp_opts\ --export
#  ;;
#esac
[[ $ENV_DEV != true ]] || usp_opts=$usp_opts\ --reload\ --reinit

#declare -p usp_opts ENV_{DEV,DEBUG,PARTS,INIT} US_PROFILE_{BASE,CORE}_SEQ >> "${US_HOST_PROFILE_CACHE:?}"
declare -p usp_opts ENV_{DEV,DEBUG} US_ENV_{PARTS,INIT} US_PROFILE_{BASE,CORE}_SEQ >> "${US_HOST_PROFILE_SHELL_CACHE:?}"

if [ -s /etc/uc/host ]
then
  . /etc/uc/host
  echo . /etc/uc/host >> "$US_HOST_PROFILE_SHELL_CACHE"
fi

if [ $ENV_SHELL != bash ]
then
  ! ((${strict:-0})) &&
    >&2 echo "Failed: Function export unavailable on ${SHELL-}" ||
    TODO "Function export unavailable on ${SHELL-}"
  return
fi

set -u

declare -gA _os_script_{load,path}
for nameref in ${US_PROFILE_BASE_SEQ}
do
  _os_script_path["$nameref"]=/usr/share/uc/$nameref.group.$ENV_SHELL
  . /usr/share/uc/${nameref}.group.$ENV_SHELL
  stat=$?
  ! ((stat)) || {
    _os_script_load["/usr/share/uc/$nameref.group.$ENV_SHELL"]=$stat
    test 0 -eq "${stat}" ||
      >&2 echo "E$_ while loading $nameref.group.$ENV_SHELL (pending)"
  }
done
unset nameref stat

: "${SCRIPTPATH_DEF:=${UC_USER_LIB}:${UC_DATA_DIR}}"
: "${SCRIPTPATH:=$SCRIPTPATH_DEF}"
# TODO: rename to SCRIPT_SET and follow uc-meta/uc-dirs
: "${SCRIPT_PATHS:=C_INC HTDOCS UCONF US_BIN U_C U_S}"

: "${UCONF:=${HOME:?}/.conf}"
: "${C_INC:=${UCONF:?}/script/composure}"

# Build SCRIPT_PATHS collection by hand (see uc-userdir)
for dirvar in ${SCRIPT_PATHS-}
do
  [[ -n "${!dirvar-}" ]] &&
  [[ -d "${!dirvar}" ]] || continue
  _SCRIPT_PATHS=${_SCRIPT_PATHS:+$_SCRIPT_PATHS }${dirvar}
  declare -x $dirvar

  for suite in ${US_TOOL_SUITES:-bash sh uc us uconf}
  do
    [[ "${!dirvar:+set}" ]] && {
      dir="${!dirvar}/tool/${suite}/part"
      [[ -d "${dir}" ]] || {
        dir="${!dirvar}/Tool/${suite}/part"
        [[ -d "${dir}" ]]
      }
    } || continue
    [[ ":${SCRIPTPATH}:" == *":${dir}:"* ]] || SCRIPTPATH=$SCRIPTPATH:${dir}
  done
done
export SCRIPT_PATHS=$_SCRIPT_PATHS
unset dir{,var} suite _SCRIPT_PATHS

for dir in ${SCRIPTPATH_DEF//:/ }
do
  [[ ":${SCRIPTPATH}:" == *":${dir}:"* ]] || SCRIPTPATH=$SCRIPTPATH:${dir}
done

export SCRIPTPATH

# Bootstrap all hand-sourced pieces so, us-part can be used to continue.
# All of the above (and other /etc/profile.d) can simply use 'export' to
# provide env for the entire session, but using a group provides a more
# structured way to keep variables and shell constructs. us-profile-env
#
User-Script.part-main --init $usp_opts ${US_PROFILE_BASE_SEQ} ||
  >&2 echo "E$? while booting (I) User-Script for /etc/profile (ignored)"

us_part $usp_opts ${US_PROFILE_CORE_SEQ} ||
  >&2 echo "E$? while booting (II) User-Script for /etc/profile (ignored)"

{
  declare -p SCRIPT_PATHS ${SCRIPT_PATHS} SCRIPTPATH
  User-Script.dump-parts --alias --export
} >> ${US_HOST_PROFILE_SHELL_CACHE}

set +u

# Id: us-profile-wrapper-dev-init                vim:set ft=bash sw=2 sts=2 et:
