This is the initial CONVENTIONS document.

- Source and generated code is primarily Bash.
- Documentation is Markdown.


# Naming conventions
Names should be strict. Some special considerations:

* Strict complang compatible ids are favorable: ``[A-Za-z_][A-Za-z0-9_]*``

- Hyphens in names refer to special and magical things. A resource, an entry
  point, an option argument. Reserve it, for example to signal that a certain
  command can exist as several identities (executable, alias, function) at
  once.

- Periods are already overloaded with meanings; its a good general purpose
  (blank) space alternative.

- Use . / ~ otherwise with the conservative meaning(s), only to be amended for
  new contexts where appropriate but not changed.

* The same goes for many special ASCII characters, their usefulness is as
  decorative mnemonic but all too many combinations will overload that.
  Recognition is a factor, also in that the smaller the presence is, the harder
  it might be to spot or find.


# Code Structure

- Organize scripts with function definitions at the top and a main execution block at the bottom.
- Function definitions are ordered alphabetically.


# Hints

- Use shell mode effectively (ie. ``set -e``, ``set -n`` or ``bash -e -n``, etc.)
- Write bashunit tests in the `test/` directory (``bashunit test/``)
