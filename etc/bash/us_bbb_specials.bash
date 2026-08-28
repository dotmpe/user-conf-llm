# Functions to inject for vanilla or "bare bone bash" environments,
# XXX: to help bootstrap
#
# a. tests?
# b. pre-processor?
# c. configure script (project/template/build), or subsequent build processes?
#
# Copyright 2026 .mpe  <me@dotmpe.com>
#
# Distributed under terms of the MIT license.
us_bbb_specials=(
  :{argv,unset}-err
  :cache-load{,maps}
  :failerr
  :pass
  :funbody
  :isfun
  say.{err,v,info,debug}
  :say-when
  :to-v
  # FIXME: ..OS.script-find
  # FIXME: ..String.globreverse-replace
)
# Id: us_bbb_specials                          vim:set ft=bash sw=2 sts=2 et:
