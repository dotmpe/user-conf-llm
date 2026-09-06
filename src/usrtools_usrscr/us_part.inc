# .inc file, see User-Conf or User-Script us-inc for specification.
# 
# Copyright 2026 .mpe  <me@dotmpe.com>
# 
# Distributed under terms of the MIT license.
._() {
: command-group
: man 'User-Scripts parts are groups for shell elements or fragments. Parts
consists of arrays of header data and static scripts that form an envelope for
a collection of functions.

# Summary
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

- preopts - literally, before us-part core applies options for part;
- global - hold globals that are not covered elsewhere, and dynamic declarations that cannot be in preopts
- load - the usual place to call or prepare things outside of local functions

While there may be custom keys in the hooks header assoc, this is the standard
lifecyle spec that us-part should follow by default.

# Further reading
For header details first read the data schema, those should explain the
mnemonics for usage and source notations.

For usage refer to the individual functions.
'
}

._hooks+load() {
: "${SCRIPTPATH:=}"
: "${US_PARTOPTS:=simple:p=0 init=0 renit:I=0 reload:L=0 export:X=0 alias:S=0 dump=0}"
: "${US_PARTOPTS:=simple:p=0 init=0 reInit=0 reLoad=0 eXport=0 aliaS=0 dump=0}"
}

.list-parts() {
: about 'Query for parts'
: param '~ [options] <Patterns...>'
: param '~ ... --ext:<.tags,>'
: param '~ ... --path:<vars,>'
: param '~ ... --ns:<maps:>'

  TODO "this is just a thin wrapper to OS expand-base for part specific context"
}

.part-main() {
: about 'Resolve and include parts'
: param '~ [options] <Parts...>'
: param '~ ... --<longopt> | -<f>'
: param '~ ... -+<longopt> | +<f>'
: param '~ ... --hooks:<names,>'
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
  local simple=0 init=0 reinit=0 reload=0 export=0 alias=0 dump=0
  hooks pathvar

  pathvar=SCRIPTPATH
  inline-fun +_parseopts
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
    [[ ${1:7} && ${1:7} == *[:,-]* ]] && {
      : "${1:7}"
      pathvars=${_//[:,]/ }
      TODO "TODO: pathvars=$_, 1:7-:${1:7}" || return
    } ||
      pathvar=${1:7}
  ;; ( --hooks[:=]* ) # Run named hook sequences
    : "${1:8}"
    hooks=${_//,/ }
  ;; -* )
      failerr "Unknown option ${1@Q}" ${_E_nss:-65} || return
  ;; * ) false
    esac
  do shift
  done
}

# Id: us_part                          vim:set ft=bash sw=2 sts=2 et:
