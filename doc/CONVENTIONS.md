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
  * TODO: document Bash-specific setup/invocation/diagnostic/triage flows, may be scenarios and cases, may be not here--but to verify exhaustive and complete workflow. And that is in parallel to working out the exact name spaces or "shell contexts" there are (cf. remarks on contexts in 'Writing scripts' section).

- Docker should help in testing scripts, and to run them in isolation on vanilla Bash setups. That allows close tabs on what is used in the dev/test runs and what is needed in deployment profile cq. bare-bone bootstrap scripts.

* For per-project setup, the package.yaml is used as a sort of "catch-all" metadata storage. It should be documented more formally (with schema), but for now roughly it is defined as a list-bag of different package.* and other JSON/TOML/YAML metadata blocks in one file--this is to keep down the clutter and try bring the work tree down to clean states.

  Certain tools will expect local dotfiles that you cannot configure, ignore files get long, and much time can go into repetitive (and boring) tool setup.
  Building and applying "profiles" of such configuration can become the task of provisioning tools (ie. Ansible) but it doesn't need to.

  With YAML, and includes or search paths for package.yaml or tools.yaml in a compatible format, collecting such file copy/patch/symlink/changeline and ad hoc scripts could be trivial.
  Redo (@config, and other targets) can help apply state from package.yaml as needed and help to achieve that goal of keeping a tidy project and work tree.

- Documentation is written in Markdown.

* Schema can be written in LinkML YAML format documents (and used to validate YAML data, or to validate or handle data in downstream projects using code generated from schema).

* This document is one of several CONVENTIONS and AGENT files, found at ``doc/`` for several projects.
  It is the primary project file for guidance and LLM/agentic interactions, of which we see two or more modes:

  - 'Full edit' or a "coding" mode, where file updates are given.
  - A conversational 'ask' mode with output restricted to examples and answers.

  In ask mode:

    - Do not restate or paraphrase the question.
    - Only add a brief note if there is a possible mismatch in topic or references.
    - Otherwise answer directly and concisely.

  In coding mode:

    - Ensure to summarize steps or actions in the answer before the actual edits.

  The CONVENTIONS document shares several parallel copies. These keep versions and editions with changes that do not fit-in with the internal structure well; the copies help to keep easy to compare parallel versions, sharing certain stanza's and overall structure while allowing to focus convention editions on a particular project and/or restricted to a specific stage and task.

- Versions are stated following semver (Semantic Versioning) 2.0.0 standard.
- In addition, TODO.txt style tags may be used inline in other formats. See
  final remarks in section 'Naming conventions' for these and more.

  TODO: want timetracking and tasks, scheduling & regime, all integrated

## Naming conventions

- In general all names should be strict: ``[A-Za-z_][A-Za-z0-9_]*``.
- Directories, file names, functions and variable names, all follow the strict rule, with some specific exceptions made on purpose.
- Filenames follow standard rules. They can have one or more .ext tags, denoting format, encoding, etc. They should have at least such extension.
  Period-prefixed names (dotnames or dotfiles) are \* nix-style hidden files and must be used as an alternative name only;
  they must not be used to duplicate file names and keep both copies, only one should exist at any one time.
- Other characters are special, but sometimes permissible.

  - Hyphens are specifically included in names to signal a special status.
  - Leading underscore are to emphasize them among or keep them distinct from others.
    Underscore-prefixing can be used to insert 'override' names sometimes, where two copies of the same name exist with the prefixed name having override status.

* Not having unified name spaces and binding means \*nix-line OS resort to PATH-like schemes for lookup,
  which may result in "shadow" names (those masked from lookup by being ordered after another directory holding identical names).
* Proper organisation of files names is described in the LHS, with projects mirroring those parts that are relevant (src, lib, etc) in the internal work tree.
  TODO: package.yaml would be the obvious place to document file tree conventions as well as other naming systems.
* Exact name formats for functions is yet to be established, and based on above considerations.
  TODO: And as separate profiles or name spaces, to-be specified by probably package.yaml combined with linkml schema.
- Since I am interested in continuing certain conventional notations here follow the most used ASCII characters.

  * `name.tag` and `.name`: Periods mark hidden names or delimit base name from extensions (file name convention). It is commonly associated with name and attribute as well ("dot" paths), for systems that have more symbols and do not need more complex relative path references and name segments.
  * `name/name`: Forward slashes are used for options, alternative sequence notations, but due to URL the most prevalent is probably that of delimiter between elements of a hierarchical or network organisation of nodes (path conventions and URL RFC's).
  * `name:tag` and `:name`: The colon marks special names and structure, it reminiscent of interactive Vim-mode command input, and also specifically associated with schema or protocols, prefix names (qnames) and with special expressions in general.
  * `name-tag` and `-n`, `-name`: The hyphen marks special names, it has become associated with resources and entities due to early WWW practices but originally reserved for command switches and options. A prefix hyphen 'hides' the argument from the other regular arguments; a double hyphen splits argument sequences (argument conventions).
    Here, they are reserved for names with several identities at once (command/function/alias/macro), or virtual groups or aliases for concrete things.

  Others:

  - +tag references a project called 'tag', should be used as the home-tag to find the repository for an entity
  - @tag references a global topic called 'tag'; global means existing in all projects
  - #tag references an unique id in a file or collection

Refer to [MANIFEST.md](MANIFEST.md) for a detailed guide on naming.

## Code Structure

- Organize scripts with function definitions at the top and a main execution block at the bottom.

  For shell scripts with executable function, it makes sense to finish shell mode changes (flags, options) before the function definitions (and initial imports).

- Function definitions are ordered alphabetically in general, but depending on the file.
  They are grouped in two sets usually, main and util but ad hoc organisations are possible.

  Certain definitions can be important in shell script file formats, those
  should appear at one established and so recognisable order.

## Writing scripts

* The intent is to finally deploy scripts that offer a good degree of confidence, and control, of the intended host/session interaction.
  But also to write succinct, and idiomatic (Bash 4.3+).
  Pre-processing will be deployed to reach these goals (`us-pp` module and command script).

  Currently there are two main context to consider for source: executable script, and parts.
  Part or part-groups (found at ``tool/*/part`` and/or in ``.group.bash``) have long function names, and the executable context uses a loader with optional exports, and aliases and name-wrapping for functions (not real Bash aliases, just optional/alternative functions generated on load).

  All current parts are fully usable (through part and handler from ``us-part`` group), but ``us-pp`` has yet to build that into a proper ``us-pp`` module+command script.

- Shells have a complex interaction of run-time mode, script and host.
  Scripts should run in strict environments but be lenient depending on context, while functions may demand strict modes/environment/etc.

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

Goal: clutter free and clear root organisation:

- Source lives in `src/` in its own (un-preprocessed) language and syntax.

  It (currently still) standard ("vanilla") Bash compatible, but not in its final Bash form yet.

* Build generates pre-distributable scripts into ``pack/ns...`` directories from source, where the ns0 and other indicate well defined formats.

- Local tooling live almost exclusively in ``tool/*/...`` where the asterisk stands-in for a globally defined suite or may be a language like "bash".
  ("tool/local" is a convenient root to tuck away any project specific scripts including Bash but without considering global or shared paths at all.)

- Other resources and dotfiles are configured (as far as possible) to be in etc/, var/, lib/, etc.

- For files that do not check in, use the .local/{etc,var,...} prefix. The .local/user is specifically to keep local user config and state.

- For cache and build, use .local/{cache,build} for local and prefer global paths. For those paths prefer to use additional subdirectories, per script or session or task, to make management easier. Do not put state information in cache, it must be regenerative and safe to be deleted without breaking the current project stage.

- Third party files need to go into ``lib/``, ``usr/`` and others as appropriate, or be kept in other trees that match the required file format and name layout.

* To list sources and targets *and* access their state externally, we use a modified redo (fork at dotmpe/redo, v0.42d+) for an ifdone command implementation. Succinctly put, the command ``redo-ifdone <target>`` makes an early non-zero exit unless the entire target branch is up-to-date. So when writing recipes (e.g. Bash, run on redo), this enables a way to cancel an invocation as invalid, and without cascading the build and explicit linking that state as prerequisite to the current (like redo-ifchange would, after executing the recipe and the rest of the branch). Simply put, it allows to say the recipe script state *could* not be valid/verified until ``<target>`` is finished, then it must restart *again* but it can not be resumed by redo. (Something redo does not capture well, is its internal--or layered-on interpreters--state. Cf. when ``*.do`` is generated from ``*.do.do``, it does not update because it does not know what makes sense perhaps, or perhaps pre-emptively by the author as an edge-case exception to be mitigated/worked around. Nb. using directories as build targets is an unbehaving edge case as well.)

  This is an addition, but serves two important purposes: it enables external integration of redo state (fail early and without any dynamic state revalidation or build cascading), and more extensive control of recipe flow wrt. target state, adding distinct bounds to the normally single unified redo state, as normally associated with project lifecycle phases or parallel setups (e.g. dev vs translation environments) as well as generative coding setups (e.g. ``*.do.do``).

## Project flow

- The basic lifecycle is src/ -> pack/ -> dist/, aided by configurable cache, build, config and other lookup/include paths.

- After preprocessing the source and intermediate is placed in pack/ns0/ initially, where those different pack/ subdirectories indicate other formatting and different name space rules and mappings.

  Also after preprocessing, further schema information is extracted about the source. 
  [TODO] Together with linkml it is the intention to document both the source structure, as well as schema for input/ouput of the scripts, and provide a means to further structure and support work on objects, adding support for instances, validating user data, working with defaults and seed data and establish common as well as specific profiles, etc. etc. etc.

To document the conventions further I need to close the loop, where the names of all calls and organizing elements are indexed and put into directory listings. And those listings (can) then refer to individual schema parts and abstractions. See 'building with data' section below.

# Writing tests

- ``bashunit test/`` or specific invocation should test important parts [to be specified].

- ``redo -k @config all`` "builds" all targets, which is conveniently configured in `.build-select.sh` to `@build @test @pack`.

  NB. special target `@config` needs be called as the very first (and explicitly), so when using parallel runners (-j<N> option) and after touching sources, make sure to run it before.

# Building with data

I'd like to generate documentation, status and other reports from schema + extracted data.

I am close to completing the transform of src/ to pack/ns0/ but that needs to happen first.
