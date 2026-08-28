# .inc file, see User-Conf or User-Script us-inc for specification.
#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
Object() {
: type module
}

.declare-fields() {
: param ' ~ <Source-prefix> <Index1> <Suffixes...> [ -- <IndexN> <Suffixes...> ]'
#: param ' ~ --reset <Source-prefix>'
: about 'Create or append fields to an object definition'
: extended 'This accumulates the {target}_fields array while uc-namekeys tracks the rest,'
: extended 'it does not declare any of the (id or field) variables. '
: input "${1:?"$(:argv-err 1 'Data prefix symbol')"}"
: input "${2:?"$(:argv-err 2 'Data index expression')"}"
: input "${3:?"$(:argv-err 3 'Data suffixes')"}"
: envout '{Source-prefix}_{fields,namerefs,namekeys,srcsyms}'
  local target=${1} suffixes context
  # XXX: Dest is static $ns_local ref, or literal value if already set
  #context=${ns_local:-\${ns_local}}
  context='${ns_local}'
  local -n __fields=${target}_fields
  ${us_arr_pre:?}add-unique uc_obj_syms "$target"
  shift
  while (($#))
  do User-Script.Array.argv-firstseq suffixes "${@:2}" ||
      test ${_E_continue:-195} -eq $? || return $_
    ${us_arr_pre:?}merge-unique ${!__fields} suffixes
    ${uc_name_pre:?}by-name-index-references "$context" "$target" "$1" "${suffixes[@]}"
    shift $(( ${#suffixes[*]} + 1 ))
    unset suffixes
    ! (($#)) || shift
  done
}

.define-standins() {
: param '~ ...'
  local _name
  local -n _type=

  for _name; do
    \builtin . <(echo "$_type.$_name () { :; }")
  done
}

.id-context() {
: param ' ~ <Source-prefix> <Key> <Index> <ID-Field-suffix> <Field-suffixes...> *[ -- <IndexN> <Field-suffixes...> ]'
  local target=${1} key=${2} index=${3} offset
  shift 3
  local -n __srcsyms=${target}_srcsyms
  [[ ${__srcsyms:+set} ]] && offset=${#__srcsyms[*]} || offset=0
  local -n __objctx=${target}_objctx
  .declare-fields "$target" "\$$key" "${@:1:1}" &&
  .declare-fields "$target" "\$$index" "${@:2}" &&
  __objctx+=( "declare -gA ${__srcsyms[offset]}" )  &&
  __objctx+=( "declare -ga ${__srcsyms[*]:offset+1}" )
}

# TODO: use uc-name
.id-context-by-name@global() {
: param ' ~ <Symbol-prefix> <Object-Key-Id> <Object-Fields...>'
: about 'Get numeric id to Field(s) using Key and set <prefix>_{obj_id,<fields..>}'
: extended 'Set global by-name variable references <prefix>_<key> using <prefix>_obj_id'
: extended 'After setting prefix once only _<prefix>_obj_id is updated, so make sure to include all fields'

  local sym_pref=${1} obj_key=${2} obj_field obj_fields=( "${@:3}" )
  local -n obj_hash="${sym_pref}"
  [[ ${obj_hash[*]:+set} ]] ||
    :failerr "No such object map found (or empty map): ${sym_pref@Q}" || return
  local -n obj_id="${sym_pref}[${obj_key}]"
  [[ ${obj_id:+set} ]] ||
    :failerr "No such ${sym_pref} object ${obj_key@Q}" || return
  local -n sym_id=_${sym_pref}_obj_id
  if [[ ! ${sym_id:+set} ]]
  then
    for obj_field in "${obj_fields[@]}"
    do
      declare -gn "${obj_field}=${sym_pref}_${obj_field}[\$_${sym_pref}_obj_id]"
    done
  fi
  #declare -g _${sym_pref}_obj_id="$obj_id"
  sym_id=$obj_id
}

.id-context-by-name() {
: param ' ~ <Symbol-prefix> <Key> <Fields...> [-- <Sub-invocation...>]'
: about 'Get numeric id to Field(s) for Key'
: extended 'Set local or global by-name variable references'

  TODO "see global variant for now"
}

.run-targets() {
: about 'Complete all of given argument sequence'
  while (($#))
  do case "${1:?}" in

  ( _:user:init )
    ;;

  ( :object-context )
    : param ' ~ ~ <Prefix> <Fields[:,]> ...'
      local prefix=${1:?"$(:argv-err 1 'Symbol prefix')"}
      : input "${2:?"$(:argv-err 2 'Symbol fields set')"}"
      declare -gA "$prefix"
      declare -gA "${prefix}_id"
      for field in ${2//[:,]/ }
      do declare -gA "${prefix}_$field"
      done &&
      shift 2
    ;;

  ( * ) :- :failerr "No such action $FUNCNAME ${1@Q}" ${_E_nsa:?}
  esac && shift || return; done
}

# Id: uc_obj                          vim:set ft=bash sw=2 sts=2 et:
