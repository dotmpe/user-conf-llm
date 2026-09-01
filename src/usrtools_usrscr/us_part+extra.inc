# .inc file, see User-Conf or User-Script us-inc for specification.
# 
# Copyright 2026 .mpe  <me@dotmpe.com>
# 
# Distributed under terms of the MIT license.
._() {
: extension
: man '

Such parts can be dumped for reloading from cache, or encapsulated in functions
for export. Parts makes exporting arrays possible, and cache reloading (nearly)
gives the effect of Bash function exports. Allowing to bypass that entire
mechanism (of export -f) is really needed to make the entire toolchain fully
effective.a


While there may be custom keys in the hooks header assoc, this is the
standard lifecyle spec that us-part should follow by default.

Those give most scripts enough room to initialize, unless there is higher
coupling. Then hooks and grp are unsufficient alone, but automatic ordering
functionality is not aimed for here. Instead pre-ordering is enforced to move
those tasks into appropiate groups, and restrict focus of us-part.

'
}
# Id: us_part+extra                          vim:set ft=bash sw=2 sts=2 et:
