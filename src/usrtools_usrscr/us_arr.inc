# .inc file, see User-Conf or User-Script us-inc for specification.
#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
Array() {
: type group
}

.add-unique() {
: about 'Add unique item(s) to array'
: about 'Use merge-unique for array inputs'
: param ' ~ <Array> <Items...>'
: group Sys/Arr
: id sh/arr-addunique
: dep sh/arr-find
: completion 'complete -A arrayvar'
: input "${1:?$(:argv-err 1 'Array name')}"
  local item
  local -n _sys_arr_adduniq=${1:?}
  for item in "${@:2}"
  do
    .find-value-index "${1}" "${item}" || _sys_arr_adduniq+=( "${item}" )
  done
}

.array-length() {
  local -n _us_arrlen_in=${1} _us_arrlen_out=${2}
  [[ ${_us_arrlen_in[*]:+set} ]] && _us_arrlen_out=${#_us_arrlen_in} ||
    _us_arrlen_out=0
}

.add-arguments-until() {
: param '(us_argv_more) ~ <Array> <Arg1...n>'
: about 'Read arguments onto array list, until end or non-zero callback $us_argv_more'
: extended 'Callback tests wether to continue at each offset'
: id sh/arr-firstseq
: completion 'complete -A arrayvar'
: input "${1:?$(:argv-err 1 'Array-name expected')}"
: input "${2:?$(:argv-err 2 'Argument sequence expected')}"
  local __si=2
  local -n __arr=${1}
  while ${us_argv_more:?} "${@:$__si}"
  do
    __arr+=( "${!__si}" )
    ((__si+=1))
  done
  [[ $# -eq $(( ${#__arr[*]} + 1 )) ]] ||
    return ${_E_continue:-195}
}

# often it makes sense have a single symbol for test, a specifically defined
# function. That filter function however will intrinsically be linked to the
# argument type. Besides .run this gives some flexible and backwards compatible
# options.
.filter-arguments-simple() {
: param '~ <Test-command> <Dest-arr> <Args...>'
  if [[ ${1:0:1} == ':' ]]; then
    #shellcheck disable=SC2206
    local _us_arr_test2=( ${1:1} )
  else
    local _us_arr_test2=( "$1" )
  fi
  ..Array.filter-arguments _us_arr_test2 "${@:2}"
}

.filter-arguments() {
: param '~ <Test-cmd-ref> <Dest-arr> <Args...>'
  local dynamic=0 value status
  if [[ ${1:1:1} == '{' ]]; then dynamic=1
  else local -n _us_arr_test=${1:?}
  fi
  if [[ ${2:+set} ]]; then
    local -n _us_arr_out=$2
  fi
  for value in "${@:3}"
  do
    status=0
    if ((dynamic)); then
      . <(echo "$_us_arr_test")  || status=$?
    else
      "${_us_arr_test[@]}" "$value" || status=$?
    fi
    ! ((status)) || {
      [[ 1 -eq $status || ${_E_next:?} -eq $status ]] && continue
      return $_
    }
    [[ ${2:+set} ]] && _us_arr_out+=( "$value" ) || echo "$value"
  done
}

.find-glob() {
: about 'Find first index matching glob expression or return 1 status'
: param '~ <Array> <Match> [<Index-dest>]'
: extended 'Look for glob match in (at value of) array'
: completion 'complete -A arrayvar'
: input "${1:?$(:argv-err 1 'Array name expected')}"
: input "${2?$(:argv-err 2 'Match expression expected')}"
  local __sys_arr_i
  local __sys_arr_index
  local -n __sys_arr_find_in=${1:?} __sys_arr_found_index=${3:-__sys_arr_i}
  for ((__sys_arr_index=0; __sys_arr_index<${#__sys_arr_find_in[@]}; __sys_arr_index++))
  do
    #shellcheck disable=2053  # allow variable as expression
    [[ "${__sys_arr_find_in[__sys_arr_index]}" == ${2} ]] &&
      __sys_arr_found_index=$__sys_arr_index && return
  done
  false
}

.find-re() {
: about 'Find first index matching RE or return 1 status'
: param '~ <Array> <Match> [<Index-dest>]'
: extended 'Look for regular expression match in (at value of) array'
: completion 'complete -A arrayvar'
: input "${1:?$(:argv-err 1 'Array name expected')}"
: input "${2?$(:argv-err 2 'Match expression expected')}"
  local i _i
  local -n __sys_arr_find_in=${1:?} __sys_arr_found_index=${3:-_i}
  for ((i=0; i<${#__sys_arr_find_in[@]}; i++))
  do
    [[ "${__sys_arr_find_in[i]}" =~ ${2} ]] &&
      __sys_arr_found_index=$i && return
  done
  false
}

.find-value-index() {
: about 'Find first index with literal value or return 1'
: param '~ <Array> <Value> [<Index-dest>]'
: extended 'Look for literal value in array'
: id sys/arr-find
: completion 'complete -A arrayvar'
: input "${1:?$(:argv-err 1 'Array name expected')}"
: input "${2?$(:argv-err 2 'Item value expected')}"
  local i _i
  local -n __sys_arr_find_in=${1:?} __sys_arr_found_index=${3:-_i}
  [[ ${__sys_arr_find_in:+set} ]] ||
    failerr "Expected non-empty search array ${1@Q}" || return
  for ((i=0; i<${#__sys_arr_find_in[@]}; i++))
  do
    [[ "${__sys_arr_find_in[i]}" = "${2}" ]] &&
      __sys_arr_found_index=$i && return
  done
  false
}

.find-value-key() {
: about 'Set key for value if found in map, or return 1'
: param '<Array> <Value> <Key-destination> ...'
: input ${1:?$(:argv-err 1 'Input map')}
: input ${2:?$(:argv-err 2 'Search value')}
: input ${3:?$(:argv-err 3 'Destination variable')}
  local -n __map_in=${1} __var_out=${3} __in='__map_in["$key"]'
  local search=${2} key
  [[ ${__map_in[*]:+set} ]] &&
  for key in "${!__map_in[@]}"
  do
    [[ ${__in-} = "${search}" ]] && __var_out=$key && return
  done
  false
}

.generate-hmap() {
: param '<Map-name> "Path/Node/Leaf" <Value>'
: about 'Put value(s) at path-like key into Map'
: extended 'Building name lists for each node-type key, ie. directory listings'
: extended 'Subsequent values for leafs are concatenated by newlines'
: id sh/hmap/gen
: input ${1:?$(:argv-err 1 'Input map')}
: input ${2:?$(:argv-err 2 'Key path')}
: input ${3:?$(:argv-err 3 'Value')}

  local i p
  local -n __us_arr_map=${1} _map='__us_arr_map["$p"]'
  set -f
  #shellcheck disable=2206  # Expand words from string substution to items
  local -a path=( ${2//\// } )
  set +f
  for (( i = ${#path[*]}; i > -1; i-- ))
  do
    [[ $i -eq 0 ]] && p='.' || {
      : "${path[*]:0:i}"
      p=${_// /\/}
    }
    [[ $i -eq "${#path[*]}" ]] && {
      [[ "${_map:+set}" ]] && _map+=$'\n'$3 || _map=$3
    } || {
      [[ "${_map:+set}" ]] && {
        [[ " ${_map} " == *" ${path[i]}/ "* ]] || _map+=" ${path[i]}/"
      } || _map="${path[i]}/"
    }
  done
}

.invert-map() {
: about 'Swap in[key]=value to out[value]=key'
: input ${1:?$(:argv-err 1 'Input map')}
: input ${2:?$(:argv-err 2 'Output map')}
  local -n __map_in=${1}
  local key
  local -n value_in="$1[\"\$key\"]"
  local -n value_out="$2[\"\$value_in\"]"
  for key in "${!__map_in[@]}"
  do
    value_out=$key
  done
}

.make-unique() {
: about 'Remove all duplicate items, leaving first occurence'
: param "<Array> ..."
: XXX would be trivial to write-in option to keep last occurence --last
: input ${1:?$(:argv-err 1 'Array name')}
  local -A __arr_unique
  local -n __arr_ref=${1}
  local index
  local -n item="__arr_ref[\$index]"
  for index in "${!__arr_ref[@]}"
  do
    [[ ! ${__arr_unique["$item"]+set} ]] && {
      __arr_unique["$item"]=
    } || {
      # Doesnt matter which we unset
      #unset "$1[$index]"
      #unset "__arr_ref[$index]"
      unset "${!item}"
    }
  done
}

.map-extensions() {
: about 'Map trailing string part by applying trailing trim (once) and appending string value'
: param '<Input-array> <Trailing-glob> <Append-string>'
: input ${1:?$(:argv-err 1 'Input array')}
: input ${2:?$(:argv-err 2 'Remove extension')}
: input ${3:?$(:argv-err 3 'Append string')}

  local _map_from=${2} _map_to=${3} _set_new
  .map-set '_set_new=${_set_in%$_map_from}\;
    [[ ${_set_new} == "${_set_in}" ]] || _set_out=$_set_new$_map_to' "${1}"
}

.map-set() {
: about 'Static script iterator'
: param '~ <Script-string> <Input-array> [<Destination-array>] ...'
: extended 'Interface to rewrite or map indexed array values'
: input ${1:?$(:argv-err 1 'Script body')}
: input ${2:?$(:argv-err 2 'Input array')}

  local _map_scr=${1} _set_idx
  local -n __arr_in=${2} __arr_out=${3:-$2}
  [[ ${3:+set} ]] && {
    local _offset=0
    [[ ! ${__arr_out[*]:+set} ]] || _offset=${#__arr_out[*]}
    local -n _set_in='__arr_in[$_set_idx]' _set_out='__arr_out[$_set_idx+$_offset]'
  } ||
    local -n _set_in='__arr_in[$_set_idx]' _set_out='__arr_in[$_set_idx]'
  for _set_idx in "${!__arr_in[@]}"
  do
    . <(echo "$_map_scr") || return
  done
}

.merge-unique() {
: about 'Add unique item(s) to array'
: about 'See add-unique for argument input'
: param ' ~ <Array> <Arrays...>'
: completion 'complete -A arrayvar'
: input "${1:?$(:argv-err 1 'Array name')}"
  local -n _sys_arr_mrguniq=${1:?}
  local -n __arr
  local item
  for __arr in "${@:2}"; do
    for item in "${__arr[@]}"; do
      .find-value-index "${1}" "${item}" ||
        _sys_arr_mrguniq+=( "${item}" )
    done
  done
}

.run() {
: about 'Simple idiomatic, static Bash script iterator for array I/O'
: param '~ <Script-string> <Input-array> [<Destination-array>] ...'
: extended 'Use array(s) for input and optional output values for script'
: extended 'Scripts can refer to or set _arr_in and _arr_out, and iterator _arr_key'
: extended 'Setting output without destination is same as setting input'

: input ${1:?$(:argv-err 1 'Script body')}
: input ${2:?$(:argv-err 2 'Input array')}

  local _arr_scr=${1} _arr_key
  local -n __arr_in=${2} __arr_out=${3:-$2}
  local -n _arr_in='__arr_in[$_arr_key]'
  local -n _arr_out='__arr_out[$_arr_key]'
  for _arr_key in "${!__arr_in[@]}"
  do
    . <(echo "$_arr_scr") || return
  done
}

.sort() {
: param "~ <Arr-in> ..."
: input "${1:?$(:argv-err 1 'Input array(s) expected')}"
  local -n __arr_in=${1}
  local -n __arr_out=${2:-$1}
  IFS=$'\n'
  :pass "$(<<<"${__arr_in[*]}" sort)" && mapfile -t ${!__arr_out} <<< "$_"
  IFS=$' \t\n'
}

.union() {
: param "~ <Arr-in...> <Arr-out>"
: id sh/arr-union
: description "Deduplicated merge, as if arrays are sets. To modify an existing array,"
: description "see make-unique. Use merge-unique to avoid building a deduplication table."
: extended "Array out can exist, but it must already be a unique set"
: completion 'complete -A arrayvar'
: input "${1:?$(:argv-err 1 'Input array(s) expected')}"
: input "${@: -1}"
: input "${_:?$(:unset-err $(($#-1)) "Output name expected")}"

  [[ $# -gt 1 ]] || return ${_E_GAE:?}
  local -A __arr_unique
  local -n __arr_in __arr_out=${*: -1:1}
  local item
  ! [[ ${__arr_out[*]:+set} ]] || {
    # Scan existing data to make dedupe map
    for item in "${__arr_out[@]}"
    do __arr_unique["$item"]=
    done
  }
  for __arr_in in "${@: 1:$#-1}"
  do
    for item in "${__arr_in[@]}"
    do
      [[ ${__arr_unique["$item"]+set} ]] || {
        __arr_out+=( "$item" )
        __arr_unique["$item"]=
      }
    done
  done
}

# Id: us_arr                          vim:set ft=bash sw=2 sts=2 et:
