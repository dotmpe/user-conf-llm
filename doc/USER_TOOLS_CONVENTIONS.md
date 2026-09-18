# +User-Tools conventions

Status: initial high-level conventions for the +User-Tools project family.

This document is intended to become `doc/CONVENTIONS.md` in the future `+User-Tools` project. Until that project is established, it is kept here as a working home for the shared conventions of the related repositories.

## Project family

The `+User-Tools` organisation covers tools for consoles and systems with interpreters. It began as a toolkit for Bash profiles and script environments, but its scope may include other interpreter-based environments over time.

The project family currently includes:

- **+User-Tools** — the high-level organisation for shared concepts, conventions, namespace definitions, formats, and semantics.
- **+User-Scripts** — the primary implementation and distribution project for reusable Bash libraries, modules, profile support, and TUI-related scripts.
- **+User-Conf** — configuration-oriented material and derived environments.
- Other derived, auxiliary, or third-party projects as the structure develops.

Repository identity and namespace identity are separate concerns. A repository may provide one or more namespace layers, while a namespace layer may be assembled from several repositories or sparse trees.

## Goals

The project family should provide a canonical source organisation that can be adopted, extended, or reduced from ad hoc local material.

The reusable core should support:

- loading on vanilla Bash;
- integration with system and user profiles;
- selectable interactive modules, including `PS1`, terminal, VTE, Bash, and user conveniences;
- text-based UI and console environments;
- layered configuration and data lookup;
- derived distributions that contain source without requiring every data collection.

The intended result is a set of local and private systems which can combine public, private, and separately licensed source or data without making private material a requirement of the public core.

## Scope

The initial implementation scope is:

- Debian and Apt-based systems;
- Bash 4.3 or later;
- Unix-style command-line tools;
- text-based and terminal-oriented workflows.

Other interpreters or platforms may be supported later, but should not complicate the initial Bash-oriented conventions.

## Source and project layout

The following directory roles are preferred, but are not an absolute requirement for every repository:

- `src/` — canonical local source tree;
- `tool/` — convenient, replaceable development and maintenance tools;
- `lib/` — reusable library material;
- `inc/` — composable include material;
- `conf/` — configuration templates and defaults;
- `schema/` — format and data definitions;
- `doc/` — conventions, reference material, and project documentation;
- `test/` — tests and verification;
- `dist/` — generated or package-oriented output.

`tool/` is considered replaceable infrastructure. It may be rewritten or replaced without changing the conceptual status of the source in `src/`.

## Layered trees and lookup paths

Projects may be combined through sparse trees, worktrees, overlays, or installed distributions. Lookup paths provide the mechanism for composing these layers.

Variables such as `US_INC_PATH` should contain colon-delimited lookup sequences. The order is significant: earlier entries provide local overrides, while later entries provide shared or fallback material.

A typical lookup order is:

1. project-local paths;
2. user or worktree paths;
3. shared +User-Tools paths;
4. +User-Scripts library paths;
5. system or third-party paths.

The exact paths are context-dependent. The convention is that lookup order must be explicit, documented, and safe when optional layers are absent.

By-name lookup may be used where a stable namespace name is more useful than a fixed filesystem location. Such lookup should retain the same precedence rules as path lookup.

## Namespace direction

The namespace model distinguishes authored source, external input, and generated products.

- `ns0` — canonical authored source and the source namespace generally;
- `ns00` — unclassified, temporary, or ad hoc script material;
- `ns0*` — other source or input collections, including system, user, and third-party material;
- `ns1..nsN` — build, packaging, deployment, or other generated products.

The namespace number describes a layer or lifecycle role; it does not replace the owning project name.

Initial concrete source contexts include:

- `ns01` — `.inc`, generic composable include fragments;
- `ns02` — `.lib`, reusable function collections with load, init, or unload conventions;
- `ns03` — `.group`, local vanilla-Bash groups of variables, functions, and scripts;
- `ns04` — `.inc.bash`, structured Bash include and formatting contexts.

Initial product contexts include:

- `ns1` — conservative generic build or deployment output;
- `ns2` — richer or variant output, including dot-hyphen forms where useful;
- `nsN` — later specialised products defined by their build context.

These names are a working vocabulary. Their exact directory and filename forms remain bound to the packaging, storage, and deployment context.

## Configuration and data

Configuration, schemas, and data are related to the source organisation but are not automatically equivalent to executable code.

The core should work with a minimal set of built-in defaults. Additional configuration or data should be discoverable through explicit paths and should be optional unless a component declares it as required.

Public, private, and third-party collections should be distinguishable by location, manifest, or both. Generated products should retain enough information to identify their source and applicable license.

## Licensing

The current licensing policy separates executable source from documentation and other material:

- scripts, libraries, tools, and tests authored for the project: **MIT**;
- project conventions and documentation: **CC BY 4.0**;
- schemas, templates, and data: explicitly identified per collection, using CC BY 4.0, CC0, or another applicable license;
- third-party material: its original license and notices are retained;
- private or user-specific data: not included in public distributions unless redistribution is permitted.

The MIT license applies to the authored script and tooling core. It should not be assumed to apply automatically to documentation, data, copied material, or generated content with a different source license.

Where practical, files should carry an SPDX identifier or be covered by a clear directory-level notice. Mixed collections should include a `README`, manifest, or licensing note describing their boundaries.

## Current status

This is pre-existing development material undergoing reorganisation. The historical +User-Scripts project remains the implementation and compatibility home for the current script collection. The future +User-Tools project will provide the clearer home for these high-level conventions and the shared namespace model.
