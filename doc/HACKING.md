---
title: Hints
status: idea
tag: lists
---

- Use shell mode effectively (ie. ``set -e/-E/-T``, and use traps. See ``set -n`` or ``bash -e -n``, etc.)
- Write bashunit tests in the `test/` directory (``bashunit test/``)
- Isolate env by docker;

  - check common profile files with other shells;
  - check with as well with blank, and default profile and consider failsafe modes.

    ``env -i bash`` (blank entire env) would expose the shell builtin default values in the session
    (consider that for scenarios to test, and to look for failsafe / graceful recovery situations).

- Stay out the distributions initialization scripts for login and window manager;

  most of it is /bin/sh, the profile we require can be exported as plain env
  variables.

  See us-env for methods of reinitializing Bash function+data env from a profile (or parent process) exported value.
