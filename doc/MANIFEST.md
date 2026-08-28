# Naming Conventions

This document provides detailed naming conventions and considerations.

## Strict Naming

Names should be strict. Some special considerations:

* Strict complang compatible ids are favorable: ``[A-Za-z_][A-Za-z0-9_]*``

- Hyphens in names refer to special and magical things. A resource, an entry
  point, an option argument. Reserve it, for example to signal that a certain
  command can exist as several identities (executable, alias, function) at
  once.

- Periods are already overloaded with meanings; it's a good general purpose
  (blank) space alternative.

- Use . / ~ otherwise with the conservative meaning(s), only to be amended for
  new contexts where appropriate but not changed.

* The same goes for many special ASCII characters; their usefulness is as
  decorative mnemonic but all too many combinations will overload that.
  Recognition is a factor, also in that the smaller the presence is, the harder
  it might be to spot or find.
