---
title: Directory conventions
subtitle: Directory organisation and lookup path conventions
status: draft
category: [ convention ]
audience: [ developer, maintainer ]
tag: filesystem lookup-path FHS Debian XDG
---

This summarizes the context for the project layout given in the [User-Tools Project conventions](USER_TOOLS_CONVENTIONS.md);
these are are its inputs and deployment destinations, outside of its own work tree.

The *work tree* is a concrete base directory,
and is used normally to refer the root of the repository (ie. for source/dev use of the term),
but it can also be used to for a more specific edition, or environment, and so be some sub tree or variant.

For naming of files (and other resources), see [Naming conventions](NAME_CONVENTIONS.md).

## Filesystem Hierarchy Standard

The FHS provides the main directory roles for paths in use on the system:

- `/usr/lib` — distribution-managed, architecture-dependent libraries and support files;
- `/usr/local/lib` — locally installed, architecture-dependent libraries;
- `/var/lib` — persistent variable state;
- `/etc` — host-wide configuration;
- `/usr/local/etc` — locally installed host-wide configuration, although its use is less consistent;
- `/run` — transient runtime state;
- `/usr/share` and `/usr/local/share` — architecture-independent data;
- `/opt/<package>` and related `/etc/opt/<package>`, `/var/opt/<package>` — add-on packages.

## Debian

Debian Policy provides addition guidelines for packaging and file ownership.
In particular:

- packaged files should not conflict with files owned by other packages;
- package-managed files generally belong under `/usr`, `/etc`, `/var`, or `/usr/share` according to their role;
- locally administered files should normally use `/usr/local` or another explicitly local location;
- packages should use package-specific subdirectories where practical;
- configuration files under `/etc` may be treated as ``dpkg`` `conffiles`;
- package-generated or package-maintained state under `/var` should have a clear ownership boundary.

## XDG Base Directory specification

> The Freedesktop.org is a project to work on interoperability and shared base technology for free-software desktop environments

| Var | description |
|:----|:----|
| ``$XDG_CONFIG_HOME`` | default: ``$HOME/.config`` |
| ``$XDG_DATA_HOME`` | default: ``$HOME/.local/share`` |
| ``$XDG_STATE_HOME`` | default: ``$HOME/.local/state`` |
| ``$XDG_CACHE_HOME`` | default: ``$HOME/.cache`` |
| ``$XDG_RUNTIME_DIR`` | system-provided runtime directory |

See also the CLI util, and use ie. ``xdg-user-dir CONFIG`` or other to retrieve keys/values.
These are configurable by the user via `user-dirs.dirs` / ``xdg-user-dirs-update``.

The first four variables have the documented `HOME` defaults when unset.
`XDG_RUNTIME_DIR` is session-specific and is normally supplied by the operating-system or login environment;
it has no equivalent `HOME` default.

NB. these do not need to exist as a separate, distinct path.
Using XDG keys and tools can help discover graphical user system setups,
and for those can give a single point of concern to govern these kinds of paths.

Standard and ad-hoc **user directory** keys can be used with XDG as well;
the keys and values normally provided (default: ``$XDG_CONFIG_HOME/user-dirs.dir``) are:

- `DESKTOP`
- `DOWNLOAD`
- `TEMPLATES` (file boilerplates)
- `PUBLICSHARE` (ie. `~/Public`, for the ops/user to configure as e.g. LAN fileshares or other)
- `DOCUMENTS`
- `MUSIC`
- `PICTURES`
- `VIDEOS`

The values are normally direct subdirectories of ``~/`` / `HOME`; default ``$HOME`` when unset or missing.

## Pre-existing mechanism to override and/or extend namespaces

- A ``.d`` directory is a collection of fragments interpreted by a named consumer.
  The consumer defines filename validity, ordering, loading or execution behavior, and failure handling.

  Pre-existing examples are `profile`, `cron`, `sudoers`, or Systemd drop-ins.

- Lookup paths (like `PATH`), are colon-separated sequences in `env` (exported profile, session defined or loaded) and imply a layered name space.

  - For `PATH`, no restrictions are imposed.
    It is not a special shell ``env`` (like `PWD`, `_`, `-`, etc.),
    and it its value can be reset, cleared, etc.

    The default value is normally set hard, to a specific value depending on the session.
    (See ``/etc/profile`` and other session files, or `PAM`, login manager, etc.)

    If unset/blank the command session cannot function normally: all full executable paths must be given at each command.

    NB. the ``hash`` `builtin` (see below) can effectively restore a lookup hash from cache or given explicit values, entirely independently of the ``PATH`` value!

  - The shell's source has to include a static default value for unset PATH as well,
    and different session files will have their own copies or slight variants.
    The following are some copies of defaults as example.

    On Debian the root session (profile) would normally be:

    ```shell
    PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
    ```

    While for Bash the default value it sets (for root but without startup files) is:

    ```shell
    PATH=/usr/local/bin:/usr/local/sbin:/usr/bin:/usr/sbin:/bin:/sbin:.
    ```

    Other users normally get a PATH setting without sysadmin programs, but including games:

    ```shell
    PATH=/usr/local/bin:/usr/bin:/bin:/usr/local/games:/usr/games
    ```

    See also [Hints](HACKING.md) on env isolation and defaults, and a failsafe profile.

  - Other scripts and commands can adjust the `env` (an array of null-terminated strings, each formatted `key` '=' `value`),
    and specifically `PATH` too.

    But only for their own session and jet to-be spawned processes, or process fork.

    Some programs will override the `PATH` or other specific keys on purpose.

  - Processes can normally access *all their parent process* `env` via ``/proc/<PID>/environ``.

  - The ``env`` key `PATH` is used by programs to perform path name lookup for commands.

    For Bash shell, the value affects some of the main `builtins`:

    - ``command``;
    - ``exec``;
    - ``source`` / ``.`` (subject to Bash's `sourcepath` behaviour).

    And others that deal with command (or source) lookup: ``type``, ``which``, ``compgen -c``.

    Bash keeps the value cached after lookup each session (see the `hash` builtin);
    command ``hash -l`` outputs lines that invoke ``hash -p ...`` again to dump+restore state.

  The operations are:

    - Find names (executable names or source), by going from left to right
      through the sequence, testing each element as base directory for names

    - Empty elements conventionally denote the current working directory;
      other colon-delimited lookup variables may define a different meaning.
      
      User-Tools should remove or reject empty elements, unless a specific behavior is defined.

      Point consider: the presence of an empty value can lead to complex behavior.

## User-Tools suite of projects

- For files of various kinds there are specific lookup path keys and sequences.

  Within User-Tools (and aside of other pre-existing lookups),
  normally such name layers should merge into a single set without name collisions.

  Unless otherwise is specified. On definition, consider:

  - whether they are extensible name spaces,
  - whether they contain name overrides/shadows,

  and how local names must chosen and structured.

- By default, User-Tools extends a lookup sequence by appending fallback locations.
  It does not silently prepend locations,
  and it does not treat earlier directories as implicit shadowing or override paths.

- An explicit lookup definition may permit:

  - prepended local paths;
  - name-specific override markers;
  - consumer-defined shadowing;
  - collision detection or first-match selection.

  For such rules, see [Naming conventions](NAME_CONVENTIONS.md).

- In general *no script should rely on specific PATH modifications* outside of its scope;
  it should use lookup keys/sequences that are specifically defined and/or predefined and shared on purpose,
  and only according to given provisions.

  The more specific the keys the more flexible they can be configured, but at the expense at the same time of extensive configuration (and more scripts, more schema, documentation?).

- Not all data fits or should be in one specific work tree.

  User data, system or service maintained data, caches, RAM tempfs storage, etc. often have their designated locations already.

- User-Tools configuration scripts rely on environment, configuration files and name or path patterns for input.

  The single point of responsibility for that setup is in the +User-Conf-Template configuration script and skeleton files.

- User-Tools projects prefer work tree bases to be more on the sparse side, with the optimum at 5-9 entries, and as many hidden or dotfiles as required or wanted.

  Sub-levels at any point should avoid both huge index listings as well as huge file size (and line length, line count). Just for a guideline and far as feasible.

  That means:

  - paths should probably be subdivided more often rather than less;
  - and more care needs to be taken selecting ignore rules to target exact sub directories or name patterns, rather than blanket-ignore entire trees.

### Current User-Tools workflow

This is a temporary section.

At this stage in development for user-tools,

+User-Scripts-Template currently provides the configuration script and skeleton files,

including bootstrap function collections/scripts

+User-Conf-LLM is the experimental playground where all source and build scripts are,
and where the latest us-pp version is.
