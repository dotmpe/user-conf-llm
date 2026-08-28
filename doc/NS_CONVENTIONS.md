---
title: Namespace conventions (+User-Conf-LLM edition)
status: draft
category: [ convention ]
audience: [ developer ]
tag:
---

## Preamble
Any word or keyword, symbol or even syntax can be implicitly or explicitly be associated with one or more entities in a schema; allowing it to integrate with knowledge systems.

For the purpose here there are schema to organise the user-tools ecosystem and its parts;
most of those parts come in the form of functions, scripts, modules and packages that will take bulk of [the namespace module's] use at this stage.

## Abstract organisation (high level view)

Let's divide name spaces into a group pre-source and post-source, and with source being (a third group) `ns0`.

Build products of that source go to `ns1..nsN`.

Such build products (pack/dist) tend to acquire certain contexts, which allows them standalone or various integrated deployments.

ns00 on the other hand signifies any unknown random script

And finally other ns0* are the other (system, user or third party) input format collections. Those may be loadable, in their own contexts, and their own pack/dist targets.

Conversion direction can be dependent on project flow and requirements

Exact structures are bound by context (packaging, storage) as well, with compositions of names in different spaces

The ideal ns0 tree uses only ns0,

but in practice it is (currently) an organisation of several ns<N> and ns0<N> spaces,

- source namespaces describe origin and intent
- build namespaces describe output and deployment
- the prefix tells the lifecycle stage, not just the file type

## Concrete script contexts (root namespace definitions)

The abstract org hides all sort of concrete bits, but the general idea is:

| ns0 | canonical authored source |
| ns00 | unclassified or ad hoc scipts |
| ns0* | all external/system/user inpuyt collections |
| ns1 | conservative generic build/deploy output |
| ns2 | richer (dot-hyphen/special name) runtime variant of ns1 |
| ns0N | further generated or specialized targets |

| ns01 .inc        include generic composure (no controlled namespace) |
| ns02 .lib        function collection with load/init/unload callback |
| ns03 .group      local, vanilla bash structured var/fun/scr.. part group |
| ns04 .inc.bash   new composure includes format with std us-pp us-fmt-inc context |

Syntax (rough sketch);
namespace delimiter, name delimiter and name formats:

| ns1 | `:: _ [a-z_][a-z0-9_]` |
| ns2 | `.  - [A-Za-z_.,:+-][A-Za-z0-9_.,:+-]` |


## Configuration and schema collection

Tagging efforts on various resource collections will expose projects and new scripts to consolidate or integrate. Schema collection is closely linked with configuration, and to build such a system effectively there need to be dedicated build trees for different layers--not just for projects.

Another factor to consider at high level is cost, in its various aspects. With cascading builds can come resource footprints that can be clearly ill-advised and disruptive if not caught out at the configuration stage. With that in mind, cost tracking in its various kinds is one of the things that user-scripts wants to be able to transparently add to any (user) command selection.

However here we are just concerned with a few parts, and that is the configuration build and the input for that. Such build can be setup as a local worktree configured from a template project that has a skeleton and seed or template files. This is how user-scripts sets up the build and CI itself for projects.

With the standalone build, we have a system of targets that we can build robustly using Redo, and that then provides useful artefacts or keys to transparently use across projects.

Finally, building out the system, status from individual projects can flow back into the collections it is part of, giving centralised, incremental, global build targets.

If set up properly. The configuration build seems like a good place to enter user schema, but layering is more advised as redo style cascading builds can be quite explosive when all variables are left unsorted and thrown into a single build's parameters. Making system and user orthogonal, and/or strictly controlled is what the build system complex should manage.

## Organising source

Since this is a pre-processor based project, the canonical source of a scripts
can take a central place, and be as bespoke as we want.

Now, a somewhat convoluted tree is used.
But for ns0, or ns01 perhaps, and given that us-pp will take over name prefixing, it seems to make more appropriate to use single functions per file, may be in the 1-1.5 screen-size (<60 lines).

## Writing scripts

...
