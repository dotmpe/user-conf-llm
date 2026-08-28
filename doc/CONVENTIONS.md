This is the initial CONVENTIONS document.

- Source and generated code is primarily Bash.
- Documentation is written in Markdown.


# Naming conventions

- In general all names should be strict: ``[A-Za-z_][A-Za-z0-9_]*``.
- Directories, file names, functions and variable names, all follow the strict rule, with some specific exceptions.
- Filenames follow standard rules. They can one or more .ext tags, denoting format, encoding, etc. And they must have at least one.
- Other characters are special, but sometimes permissible. Hyphens are specifically included in names to signal a special status.
  
Refer to [MANIFEST.md](MANIFEST.md) for a detailed guide on naming.


# Code Structure

- Organize scripts with function definitions at the top and a main execution block at the bottom.
- Function definitions are ordered alphabetically.


# Hints

- Use shell mode effectively (ie. ``set -e``, ``set -n`` or ``bash -e -n``, etc.)
- Write bashunit tests in the `test/` directory (``bashunit test/``)
