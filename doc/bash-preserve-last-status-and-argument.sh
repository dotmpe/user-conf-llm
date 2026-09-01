#!/bin/bash
#
# bash-preserve-last-status-and-argument.sh
#
# Distributed under terms of the MIT license.

pass () {
  declare stat=${?} last=${_}
: param '~ ...'  # The tilde placeholds for "the current thing", like a dict entry
  >&2 declare -p stat last
  # restore $_
  : "$last"
  # restore $?
  return ${stat}
}

# Status is passed along. Hidden last arguments are possible, but only when the
# specific invocation allows for that. In User-Script the toolkit approach is to
# document such behavior in the `param` "meta key", as illustrated above. When
# the initial param line specifies '...' at the very end, its telling that
# trailing arguments are ignored, and hidden arguments supported. Absence of
# this indicates unspecified behavior or E:GAE (generic-argument-error) status
# triggers.
false last
pass $_
echo stat=$? last=$_

# When inlining scripts, status *and* last argument capture and preservation can
# become relevant. Observe however this works, and it does run to the end
# because of our set +e mode shell. (The added ... && ... || ... expression is
# to make explit that the funbody dereferencing is working.) For real inlining,
# that needs heuristics to decide how to join pieces. Either all pieces go in
# a '{ ...; } && { ...; } chain, or some scanning or convention indications when
# ' || return' suffixing is needed. The latter would be implicit at the end of a
# function normally, but senseless after a if/fi or true/:, for example.
#
# NB: ((...)) and keywords do not change last-argument as builtin/special or
# real commands do. Diagnostics or testing tool scripts however can and do
# interfere, so it's good to be aware of the exact behavior and limitations.
inner() {
  echo stat=$? last=$_
}
false last
. <(
  sh_funbody pass &&
  sh_funbody inner || exit 3
);
true # status was false indeed
