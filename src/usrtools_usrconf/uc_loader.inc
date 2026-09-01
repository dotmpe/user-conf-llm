Loader() {
: grp us-os us-str uc-cache uc-loader-meta
}

._hooks() { : -A : ; }
._hooks:global() {
  declare -ga uc_inc_init
  declare -gA uc_inc_meta
}
._hooks:load() {
  # Setup include path for uc-loader
  : "${UC_INC_DIR:=${C_INC:?}}";
  [[ -d $PWD/src ]] &&
  : "${UC_LOADER_DIR:=$PWD/src}" ||
  : "${UC_LOADER_DIR:=${C_INC:?}}"
  : "${UC_INC_PATH:=${UC_INC_DIR:?}}"
  append_lookup "$UC_INC_DIR" "$UC_LOADER_DIR" UC_INC_PATH
  [[ ! -d $PWD/src ]] || append_lookup "$PWD/src" UC_INC_PATH
}

.expand-source() {
: about 'Shell source include preprocessor'
: param '~ <Source-path> [<Root-dir>] ...'
: tag util
: input "${KEEP_META:=1}"
  (($#)) || return ${_E_MA:-194}
: input "${1:?$FUNCNAME${*:+ ${*@Q}}: Source path expected, $ENV_CTX}"
  local source_{base,line,name,path,ref} loader_dir base absref ext=inc _{1..5} local_names

# 1. read file and expand function names against source (base) name
# - . or ./ or .%.inc explicitly becomes main function associated with name
# more names become ./... further sub paths
# 2. should expand other magic names as well, and recognize "$@" or exec
# branch forms, for wrapping/conversion of scripts. But more advanced rewrites
# are may be for later, can use shell aliases and typeset expansion for now
# I think.

  # TODO: check for not absolute/outside; probably handle multiple roots/include path

  loader_dir="${2:-${UC_LOADER_DIR-}}"
  [[ ${loader_dir:+set} && -d "$loader_dir" ]] ||
    failerr "Source root directory needed" || return

  source_ref=${1}
  source_path=$(PWD=$loader_dir realpath -m "${source_ref}") ||
  #source_path=$(realpath -m --relative-to "$loader_dir" "${1}") ||
    failerr "Cannot resolve ${1@Q} against ${loader_dir@Q}" || return

  source_base=${source_ref%".$ext"}
  #source_name=${source_base##*/}

  while IFS=$'\n' read -r source_line; do
    # skip empty, comment and blank (space/tab) source lines
    [[ ${source_line:+set} &&
      ${source_line} != \#* &&
      ${source_line} = *[!$'\t ']* ]] || continue
    # scan loop of first character for initial and then subsequent action
    while case "${source_line:0:1}" in

      ( \  )
          ...String.globstrip-charsleft source_line $'[\t ]'
        ;;

      ( __ ) source_line=./\ ${source_line:3} ;;
      ( [%A-Za-z_-][A-Za-z0-9_\ -] ) source_line=./$source_line ;;

      ( . ) 
          _1="${source_line%'() {'}"
          _2="${_1%% *}/"
          _3="${_2//"./"}"
          _4="${_3:-.}"
          _5="${_4%/}"
          _6="${_5//[%.]/"${source_base##*/}"}"
          _7="/${source_base}${_6:+/$_6}"
          [[ ${_7} =~ [A-Za-z_][A-Za-z0-9_]* ]] ||
            failerr "Name violation ${_7@Q}" || return
          local_names+=( "$_7" )
          echo "${_7} () {"
          continue 2
        ;;
      ( :\  )
          if \
            .Meta.resolve-hook "/${source_base} ${source_line:2} "
          then
            : ok ...
          else
            uc_inc_meta["${source_line:2}"]+=/$source_base$'\n'
          fi
          ((KEEP_META)) || continue 2
          break
        ;;
      ( \ \  | \}* ) break ;;
      ( [!\ ]* ) TODO "Unexpected syntax for $ext file" ;;
      ( * ) failerr "Unexpected line ${source_line@Q}" || return ;;
    esac; do continue; done
    echo "$source_line"
  done < "$loader_dir/$source_base.$ext" ||
    failerr "E$? reading ${source_name@Q}" || return
  if [[ ${uc_inc_meta[*]:+set} ]]; then
    declare -p uc_inc_meta
  fi
  declare -p local_names loader_dir
  say.debug 'Expand-source finished'
  echo ">&2 echo \"definition loaded\""
  if [[ ${uc_inc_init[*]:+set} ]]; then
    printf '__uc_loader () { >&2 echo "now in __uc_loader"; %s; }
 __uc_loader "${local_names[@]}"\n' "${uc_inc_init[@]}"
  fi
  unset uc_inc_init
}

.resolve() {
: about 'Load one include with function(s)'
: param '~ <Include-name> ...'
: tag util
: env _E_MA UC_LOADER_PATH UC_INC_PATH
  (($#)) || return ${_E_MA:?}
: input "${1:?$FUNCNAME: Include name expected, $ENV_CTX}"
  local source_{name,path} include_name nameoffset ext=inc
  local -n lookup_path=${UC_LOADER_PATH:-UC_INC_PATH}
  source_name=$1
  include_name=$source_name.$ext

  ../OS.path-find "${!lookup_path}" "$include_name" source_path &&
  ((nameoffset=${#source_path}-${#include_name}-1)) && . <(
    .expand-source "${source_path:?}" "${source_path:0:nameoffset}"
  )
}
