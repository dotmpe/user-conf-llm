# Project conventions

I am developing a generative shell-script system for command-substitution tooling (User-Scripts). I want rich prose documentation, explicit schemas for the generative parts, and a path toward using Lua for data, heuristics, rules, testing, and diagnostics. Help me explore the design space, propose schemas, and produce both explanatory text and concrete Bash (and later Lua) artifacts.

## Getting started

- I have detailed personal preferences (see [PREFERENCES](PREFERENCES.md)) that affect the toolchain choices, but they boil down to: in-terminal, but open to graphical (Xorg) power tools if they are universal and long-term viable.

* We want to start by using the Bash interpreter to its full extent, and not getting in the way of the existing system. That means learning about and sometimes dealing with /bin/sh and other shells.

  * Bash goes well with redo (apenwarr/redo), a principled generic \*nix build tool.
  * Bash scripts need a test framework, Bashunit is the runner and assertions for unit testing our scripts.

- Docker should help in testing scripts, and to run them in isolation on vanilla Bash setups. That allows close tabs on what is used in the dev/test runs and what is needed in deployment profile cq. bare-bone bootstrap scripts.

- Documentation is written in Markdown.
- Tasks are written in TODO.txt.
- Versions are stated following semver (Semantic Versioning) 2.0.0.
- In addition, TODO.txt style tags may be used inline in other formats. See
  final remarks in section 'Naming conventions' for these and more.

* For per-project setup, the package.yaml is used as a sort of "catch-all" storage. It should be documented more formally (with schema), but for now roughly it is defined as a list-bag of different package.* and other JSON/TOML/YAML metadata blocks in one file--this is to keep down the clutter and try bring the work tree down to clean states.

  Certain tools will expect local dotfiles that you cannot configure, ignore files get long, and much time can go into repetitive (and boring) tool setup.
  Building and applying "profiles" of such configuration can become the task of provisioning tools (ie. Ansible) but it doesn't need to.

  With YAML, and includes or search paths for package.yaml or tools.yaml in a compatible format, collecting such file copy/patch/symlink/changeline and ad hoc scripts could be trivial.
  Redo (@config, and other targets) can help apply state from package.yaml as needed and help to achieve that goal of keeping a tidy project and work tree.

In ask mode:
- Do not restate or paraphrase the question.
- Only add a brief note if there is a possible mismatch in topic or references.
- Otherwise answer directly and concisely.

## Naming conventions

- In general all names should be strict: ``[A-Za-z_][A-Za-z0-9_]*``.
- Directories, file names, functions and variable names, all follow the strict rule, with some specific exceptions made on purpose.
- Filenames follow standard rules. They can one or more .ext tags, denoting format, encoding, etc. And they must have at least one.
- Other characters are special, but sometimes permissible. Hyphens are specifically included in names to signal a special status.
* Not having unified name spaces and binding means \*nix-line OS resort to PATH-like schemes for lookup,
  which may result in "shadow" names (those masked from lookup by being ordered after another directory holding identical names).
* Proper organisation of files names is described in the LHS, with projects mirroring those parts that are relevant (src, lib, etc) in the internal work tree.
  package.yaml would be the obvious place to document file tree conventions as well as other naming systems.
* Exact name formats for functions is yet to be established, and based on above considerations.
  And as separate profiles or name spaces, to-be specified by probably package.yaml combined with linkml schema.
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

# Code Structure

- Organize scripts with function definitions at the top and a main execution block at the bottom.

  For shell scripts with executable function, it makes sense to finish shell mode changes (flags, options) before the function definitions (and initial imports).

- Function definitions are ordered alphabetically in general, but depending on the file. They are grouped in two sets usually, main and util but ad hoc organisations are possible.

  Certain definitions can be important in shell script file formats, those
  should appear at one established and so recognisable order.

# Writing scripts

The basic lifecycle is src/ -> pack/ -> dist/.

The source goes into src/ and is standard ("vanilla") Bash compatible, but not in its final Bash form yet.

After preprocessing the source and intermediate is placed in pack/ns0/ initially, where those different pack/ subdirectories indicate other formatting and different name space rules and mappings.

Also after preprocessing, further schema information is extracted about the source. 
[TODO] Together with linkml it is the intention to document both the source structure, as well as schema for input/ouput of the scripts, and provide a means to further structure and support work on objects, adding support for instances, validating user data, working with defaults and seed data and establish common as well as specific profiles, etc. etc. etc.

To document the conventions further I need to close the loop, where the names of all calls and organizing elements are indexed and put into directory listings. And those listings (can) then refer to individual schema parts and abstractions. See 'building with data' section below.

# Writing tests

- ``bashunit test/`` or specific invocation should test important parts [to be specified].

- ``redo -k @config all`` "builds" all targets, which are conveniently configured in `.build-select.sh` to `@build @test @pack`.

  Special target `@config` needs be called as the very first (and explicitly), so when using parallel runners (-j<N> option), make sure to run it before.

# Building with data

I'd like to generate documentation, status and other reports from schema + extracted data.

I am close to completing the transform of src/ to pack/ns0/ but that needs to happen first.
