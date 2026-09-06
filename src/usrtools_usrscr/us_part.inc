# .inc file, see User-Conf or User-Script us-inc for specification.
#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
Part() {
: command-group
: man 'User-Scripts parts are groups for shell elements or fragments. Parts
consists of arrays of header data and static scripts that form an envelope for
a collection of functions.

## Summary
The main purposes of ~ are:

- loading variable/function/alias (type of) sets as groups from source
- reinitializing such groups from cache, with complete track of all moving parts
- to represent either library or runcom/profile type scripts the same, and
  aid in aggregation and layering.

# Brief description of operation
Part options control the core loop, that runs on pre-ordered sequences of part
names. There is a grp array in the header, to declare additional names for
suffixing to the current load sequence.

Hooks are triggered next first, and again two more times after the entire
sequence has run. The phases are:

- preopts - literally, before us-part core applies options for part\;
- global - hold globals that are not covered elsewhere, and dynamic declarations that cannot be in preopts
- load - the usual place to call or prepare things outside of local functions

While there may be custom keys in the hooks header assoc, this is the standard
lifecyle spec that us-part should follow by default.

## Further reading
For header details first read the data schema, those should explain the
mnemonics for usage and source notations.

For usage refer to the individual functions.
'
}

._hooks() { : -A : ; }
._hooks:global() {
: env "${SCRIPTPATH:=}"
# : env "${US_PARTOPTS:=simple:p=0 init=0 renit:I=0 reload:L=0 export:X=0 alias:S=0 dump=0}"
# : env "${US_PARTOPTS:=simple:p=0 init=0 reInit=0 reLoad=0 eXport=0 aliaS=0 dump=0}"
}

.list-parts() {
: about 'Query for parts'
: param '~ [options] <Patterns...>'
: param '~ ... --ext:<.tags,>'
: param '~ ... --path:<vars,>'
: param '~ ... --ns:<maps:>'

  TODO "this is just a thin wrapper to OS expand-base for part specific context"
}

.main() {
  .part-main "$@" \;
}

.part-main() {
: about 'Resolve and include parts'
: param '~ [options] <Parts...>'
: param '~ ... --<longopt> | -<f>'
: param '~ ... -+<longopt> | +<f>'
: param '~ ... --hooks:<names,>'
: env DEBUG DIAG
: man '~ is an rudimentary argument parser followed by a load/cache/option loop
  over source part names.

  Tasks:
  1. lookup and resolve for name and id, and using regular path lookup and pattern mapping
  2. record the one found name-to-id as a group sharing declared prefix <pre>; and
  3. save the found state in a part cache function (with header copy, kept outside the source)
  4. run each option: grp, var, ssc, als

  For details on schema read the us_part linkml specification.

  ~ is a core routine, that depends on other core parts:

  User-Script.resolve-name
  User-Script.OS.source-once
  User-Script.Scripting.inline-callbacks  # combine ssc/als and funbody inline
  '
: alias us-part
  local simple=0 init=0 reinit=0 reload=0 export=0 alias=0 dump=0 hooks pathvar
  local -a load_scripts
  inline-fun +_parseopts
  local name{global,tag,ref,word} partname funname
  local -n _namesrc='_os_script_path["$nameref"]'
  [[ ${_os_script_path[*]:+set} ]] || declare -gA _os_script_path
  while (($#)); do
    nameref=${1?$(:argv-err 1 'Part name')}
    nameword=${nameref//[^A-Za-z0-9_]/_}
    nametag=
    local -n \
      partpref=${nameword}_pre \
      partgrp=${nameword}_grp \
      partvar=${nameword}_var \
      partfun=${nameword}_fun \
      partssc=${nameword}_ssc \
      partals=${nameword}_als \
      parthooks=${nameword}_hooks
    nameglobal=${partpref:-${nameref}}
    ! ((reinit || reload)) && >/dev/null 2>&1 declare -F ${nameref}@part && {
      ${nameref}@part
    } || {
      ((init && !reload)) || {
        [[ ${_namesrc:+set} ]] ||
          US_SCR_PATH=$pathvar \
          .resolve-name "${nameref}" "${!_namesrc}" nametag ||
            :failerr "E$? while resolving $nameref name" || return
        ! ((reload )) && {
          ..OS.source-once "${_namesrc}" ||
            :failerr "E$? while loading $nameref source" || return
        } ||
          \builtin . "${_namesrc}" ||
            :failerr "E$? while resolving $nameref part source" || return
      }
      ! ((DEBUG)) || {
        [[ ${partpref:+set} || ${partgrp[*]:+set} || ${partvar[*]:+set} ||
            ${partfun[*]:+set} || ${partssc[*]:+set} || ${partals[*]:+set} ||
            ${parthooks[*]:+set} \
        ]] ||
          :failerr "Part ${nameref} appears to be empty (ignored)"
      }

      # There are cases where prefix is not needed
      if [[ ${partpref:+set} ]]; then
        [[ ${partpref: -1:1} == . ]] || partpref=$partpref.
      fi

      ! ((reload || reinit)) &&
      >/dev/null 2>&1 declare -F ${nameref}@part ||
        eval "${nameref}@part () {
: prefix ${nameglobal}
  ${nametag:+: tag ${nametag}
}  $(for k in ${nameword}_{pre,grp,var,fun,ssc,als,hooks}
      do test -n "$(2>/dev/null declare -p $k)" || continue
        : "${_:9}"
        : "${_#-}"
        echo "declare -g${_}"
      done)
}"
    }

    if ! ((simple)) && [[ ${partgrp:+set} ]]; then
      set -- "${partgrp[@]}" "$@"
      continue
    fi

    if [[ ${hooks:+set} && ${parthooks[*]:+set} ]]; then
      local hook
      for hook in ${hooks}; do
        test -n "${parthooks["$hook"]-}" || continue
        if [[ $hook = load ]]; then load_scripts+=( "$_" )
        else
          \builtin . <(printf '%s\n' "$_") ||
            :failerr "E$? during $nameref hook ${hook@Q} (ignored)"
        fi
      done
    fi

    local -a funex varex
    [[ ! ${partfun:+set} ]] || for partname in "${partfun[@]}"; do
        : "${partname:?Name for $nameref partssc}"
        [[ ${partname:0:1} == . ]] && : "${partpref-}${partname:1}" || : "${partname}"
        funex+=( "${_}" )
      done
    [[ ! ${partvar:+set} ]] || for partname in "${partvar[@]}"; do
        varex+=( "${partname:?Name for $nameref partvar}" )
      done
    [[ ! ${partssc[*]:+set} ]] || for partname in "${!partssc[@]}"; do
        : "${partname:?Name for $nameref partssc}"
        # Ignore all special aliases here
        [[ ${partname:0:1} == [A-Za-z_.] ]] || continue
        [[ ${partname:0:1} == . ]] && : "${partpref-}${partname:1}" || : "${partname}"
        funname=${_}
        ! ((reinit)) &&
        >/dev/null 2>&1 declare -F ${funname} || {
          [[ ${partssc["$partname"]:0:1} = . ]] &&
          : "${partpref-}${partssc["$partname"]:1}" ||
            : "${partssc["$partname"]}"
          eval "$funname () { ${_:?}; }"
        }
        funex+=( "${funname}" )
      done
    ! ((alias)) || for partname in "${!partals[@]}"; do
        # XXX: (almost) completely the same as ssc for now
        : "${partname:?Name for $nameref partals}"
        # Ignore certain special aliases by default
        [[ ${partname:0:1} == [A-Za-z_.] ]] || continue
        [[ ${partname:0:1} == . ]] && : "${partpref-}${partname:1}" || : "${partname}"
        funname=${_}
        ! ((reinit)) &&
        >/dev/null 2>&1 declare -F ${funname} || {
          [[ ${partals["$partname"]:0:1} = . ]] &&
          : "${partpref-}${partals["$partname"]:1}" ||
            : "${partals["$partname"]}"
          eval "$funname () { ${_:?} \"\$@\"; }"
        }
        funex+=( "${funname}" )
      done

    ! ((DEBUG || DIAG)) || {
      for var in "${varex[@]}"; do
        :pass "$(2>/dev/null declare -p "${var}")" || [[ ${_:+set} ]] ||
          :ignore :failerr "Missing variable ${var}"
      done
      for fun in "${funex[@]}" "${nameref}@part"; do
        :pass "$(2>/dev/null declare -F "${fun}")" || [[ ${_:+set} ]] ||
          :ignore :failerr "Missing function ${fun}"
      done
    }

    ((dump)) && {
      [[ ! ${varex:+set} ]] || :- ..Shell.dump-globals "${varex[@]}"
      declare -f "${funex[@]}" "${nameref}@part"
      ! ((export)) || echo "declare -fx ${funex[*]} ${nameref}@part"
    } ||
      ! ((export)) || {
        [[ ! ${varex:+set} ]] ||
          declare -gx "${varex[@]}"
        declare -fx "${funex[@]}" "${nameref}@part"
      }

    [[ ! ${hooks:+set} || ! ${parthooks[*]:+set} ]] || {
      local hook
      for hook in ${hooks}; do
        test -n "${parthooks["$hook-after"]-}" || continue
        \builtin . <(printf '%s\n' "${_}") ||
          :failerr "E$? during $nameref after hook ${hook@Q} (ignored)"
      done
    }

    unset varex funex
    shift
  done
  if [[ ${load_scripts[*]:+set} ]]; then
    \builtin . <(printf '%s\n' "${load_scripts[@]}") ||
      :failerr "E$? during post hooks (ignored)"
  fi
}

.part-main+_parseopts() {
  while case "${1:?}" in
     ( --simple ) simple=1 # Don't try to look for included groups
  ;; ( --init ) init=1 # Don't source part, assume it is present and run other options
  ;; ( --reload ) reload=1
  ;; ( --reinit ) reinit=1 # Don't reuse @part function
  ;; ( --export ) export=1 # Export functions and vars
  ;; ( --alias ) alias=1 # Create functions from static script and function name map
  ;; ( --dump ) dump=1 simple=1 hooks=dump # Typeset to out
  ;; ( --path[:=-]* ) # Specificy lookup PATH-var(s)
      pathvar=${1:7} && [[ ${!pathvar:+set} ]] ||
        :failerr "No value for lookup path ${pathvar}" || return
  ;; ( --hooks[:=]* ) # Run named hook sequences
      : "${1:8}"
      hooks=${_//,/ }
  ;; -* )
      :failerr "Unknown option ${1@Q}" ${_E_nss:-65} || return
  ;; * ) false
    esac
  do shift
  done
}

.resolve-name() {
: param '~ <Name> <Dest-Var> <Id-var> <Exts...>'
: input "${1?$(:argv-err 1 'Name value')}"
: input "${2?$(:argv-err 2 'Path dest var name')}"
: input "${3?$(:argv-err 3 'Id dest var name')}"

  ..OS.script-find "" "${2}" "${1}" "${@:4}" || {

    test 127 -eq $? || :failerr "E$_ while loading $1" $_ || return

    local -n _32591e26_tagname=${3}
    ..String.globreverse-replace - , "${1}" "${3}" &&
    ..OS.script-find "" "${2}" "${_32591e26_tagname}" "${@:4}" || {
      test 127 -eq $? ||
        :failerr "E$_ while loading $1 <$_32591e26_tagname.bash>" $_ || return
      :failerr "E$_ $FUNCNAME cannot find $1 <$_32591e26_tagname.bash>" 127 || return
    }
  }
}

# Id: us_part                          vim:set ft=bash sw=2 sts=2 et:
