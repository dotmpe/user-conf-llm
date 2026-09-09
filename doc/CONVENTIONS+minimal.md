# Project conventions

Preamble:
: I am developing a generative shell-script system for command-substitution tooling (User-Scripts). I want rich (inline and contextually structured) documentation, and dense or (controlled) natural language [CNL] for readable strings, and explicit schemas for the generative parts, and (eventually too) a path toward using Lua for faster data and heuristics, and to further explore optimization through elimation of Bash, or by layering of Bash on Lua, or by deferring to compiled spin-off projects. I need help better testing, documenting flows, and to keep a low-key approach while developing solid tools and habits for writing source, making distributions, doing deployment, and managing environments for testing, diagnostics, etc. etc.
  Help me explore the design space, propose schemas, and produce both explanatory text and concrete artifacts.

## Getting started

- The current choice of tool chain favors \*nix style tools, with in-terminal, text-based UI.

* We want to start by using the classic shell interpreter to its full extent, and not getting in the way of the existing system. That means learning about and sometimes dealing with /bin/sh and other shells.

  * The body of work is already written in Bash, and we should target a modern (4.3+) version to avoid needing to carefully pick features prematurely (cf. pre-processing goals that can alleviate/mitigate some constraints).
  * Bash goes well with redo (apenwarr/redo v0.42d+), a principled generic \*nix build tool.
  * Bash scripts need a test framework, Bashunit (bashunit.com / TypedDevs, v0.33+) is the runner and assertions provider for unit testing scripts.
- Documentation is written in Markdown.

* Schema can be written in LinkML YAML format documents (and used to validate YAML data, or to validate or handle data in downstream projects using code generated from schema).

* This document is one of several CONVENTIONS*md files, found at ``doc/`` for several projects. It is the primary project file for guidance and LLM/agentic interactions, of which we see two or more modes:

  - 'Full edit' or a "coding" mode, where file updates are given.
  - And a conversation-restricted 'ask' mode with output restricted to answers.

  In ask mode:

    - Do not restate or paraphrase the question.
    - Only add a brief note if there is a possible mismatch in topic or references.
    - Otherwise answer directly and concisely.

# Writing scripts

* The intent is to finally deploy scripts that offer a good degree of confidence, and control, of the intended host/session interaction. But also to write succinct, and idiomatic (Bash 4.3+). Pre-processing will be deployed to reach these goals (`us-pp` module and command script).

  Currently there are two main context to consider for source: executable script, and parts. Part or part-groups (found at ``tool/*/part`` and/or in ``.group.bash``) have long function names, and the executable context uses a loader with optional exports, and aliases and name-wrapping for functions (not real Bash aliases, just optional/alternative functions generated on load).

  All current parts are fully usable (through part and handler from ``us-part`` group), but ``us-pp`` has yet to build that into a proper ``us-pp`` module+command script.

- Shells have a complex interaction of run-time mode, script and host. Scripts should run in strict environments but be lenient depending on context, while functions may demand strict modes/environment/etc.

  * For that reason, (for now) it makes sense to use \ (backslash escaping) and ``builtin`` (to defeat aliasing and function name "shadowing" the actual command), beside the ``command``- builtin (to default both), to **guard** *shell invocations* in case of unstrict/name-aliasing modes. For awareness, ``eval``, ``.``/``source``, and ``read*``-variants are normally "guarded" by `\builtin` until context can be validated and satisfied explicitly.
    (NB. Strictly speaking, there is no reason to also "guard" local, declare, and all builtins, other than that this would be too noisy at this stage.)

  * Run-time also affects input checks, and again the explicit measure is preferred: use `:?` (or `?`) on all variables and argument positions, at least the first time of use in a script.

    This syntax is because of this tied together with a certain inline documentation for the parameters for a certain type of utility functions. Alternatively ``(($#)) || return ${_E_MA:?}``, or ``return ${_E_GAE:?}`` can be used for 'missing-arguments' and 'generic-argument-error' scenarios, but they would be less informative (in a vanilla Bash session) without more setup and handling around it. (Same goes for other, non-variable related environment checks, but the error status range and signals for those are to-be-determined.)

    Same goes for required as for unset and default, use ``-`` or ``:-def`` explicitly.

    And ``:+not-empty`` or ``+not-unset`` (or short mnemonic string) for test/``[[`` expressions.

* Variables for function namespace prefixes are not usable to define functions. For example, this does not work (and is the reason for opting for pre-processing):

```bash
us_mod_pre=My.Fancy.Long.Name.Space.Prefix
${us_mod_pre}.MyFun() { :; }
```
  
  But these variables are convenient to keep, for calling functions still as naming later (ie. packaging/distribution) will be adaptive/ad-hoc, and it just makes sense for the current context (part group files and main).

```pseudo
us_mod_pre=Still.Long.Name.Space.Prefix.
Still.Long.Name.Space.Prefix.MyFun() {
  ${us_mod_pre:?}MyOtherLocalFun <arguments...>
}
```

* For flags, options or any sort of symbol or key we prefer the full or readable form over the flag or mnemonic forms.
  Mnemonics may be appropriate according to context, but never the flag form unless that is the only input.
  We do not want to write code for the initiates only unnecessary, only when context demands it (e.g. regular expressions, file permissions or shell mode and other parsed strings, etc.).

# Project Setup

- Source lives in `src/` in its own (un-preprocessed) language and syntax.

  It (currently still) standard ("vanilla") Bash compatible, but not in its final Bash form yet.

* Build generates pre-distributable scripts into ``pack/ns...`` directories from source, where the ns0 and other indicate well defined formats.

  Local tooling live almost exclusively in ``tool/*/...`` where the asterisk stands-in for a globally defined suite or may be a language like "bash".
  ("tool/local" is a convenient root to tuck away any project specific scripts including Bash but without considering global or shared paths at all.)

  Other resources and dotfiles are configured as far as possible to be in etc/, var/, lib/, etc.

  For files that do not check in, use the .local/{etc,var,...} prefix. The .local/user is specifically to keep local user config and state.

  For cache and build, use .local/{cache,build} for local and prefer global paths. For those paths prefer to use additional subdirectories, per script or session or task, to make management easier. Do not put state information in cache, it must be regenerative and safe to be deleted without breaking the current project stage.

* To list sources and targets *and* access their state externally, we use a modified redo (fork at dotmpe/redo, v0.42d+) for an ifdone command implementation. Succinctly put, the command ``redo-ifdone <target>`` makes an early non-zero exit unless the entire target branch is up-to-date. So when writing recipes (e.g. Bash, run on redo), this enables a way to cancel an invocation as invalid, and without cascading the build and explicit linking that state as prerequisite to the current (like redo-ifchange would, after executing the recipe and the rest of the branch). Simply put, it allows to say the recipe script state *could* not be valid/verified until ``<target>`` is finished, then it must restart *again* but it can not be resumed by redo. (Something redo does not capture well, is its internal--or layered-on interpreters--state. Cf. when ``*.do`` is generated from ``*.do.do``, it does not update because it does not know what makes sense perhaps, or perhaps pre-emptively by the author as an edge-case exception to be mitigated/worked around. Nb. using directories as build targets is an unbehaving edge case as well.)

  This is an addition, but serves two important purposes: it enables external integration of redo state (fail early and without any dynamic state revalidation or build cascading), and more extensive control of recipe flow wrt. target state, adding distinct bounds to the normally single unified redo state, as normally associated with project lifecycle phases or parallel setups (e.g. dev vs translation environments) as well as generative coding setups (e.g. ``*.do.do``).

# Project flow

- The basic lifecycle is src/ -> pack/ -> dist/.

- ``redo -k @config all`` "builds" all targets, which is conveniently configured in `.build-select.sh` to `@build @test @pack`.

  NB. special target `@config` needs be called as the very first (and explicitly), so when using parallel runners (-j<N> option) and after touching sources, make sure to run it before.
