---
title: Tag conventions (full +User-Conf-LLM edition)
status: idea
category: [ convention ]
audience: [ developer ]
tag: sketch extension
---

# Tag conventions

Tag are designated, decorated mnemonics for organizing any sort of entity. They
are used in various modules for queries and rules, much like options on a CLI.

When you tie tag very closely to a script you can build a sort of rule system,
or a build or resource cq. entity organisation, or all in one. Part of this
could be made with a Redo configuration, which deals with virtual target
organisations like this very well:

  \?         Run help
  \?\?       Show how help works
  \?<tag>    Run help on tag entity
  \?\?<tag>  Show how tag entity works
  @          Run global context, or some global tag list, or walker, or summary
  @<tag>     Run tag context
  -          Close context
  -<tag>     Close tag context
  +          Open/add ...
  +<tag>
  .          Call ...
  .<tag>
  /          Dir (list) ...
  /<tag>

For build systems and in generative, incremental projects, having scripts that
can typeset themselves is a very useful feature.

Tag have to coordinate their language across project contexts,
ie. mnemonic usage through mapped name spaces.

For this all the user's tag and all the other source terms are assigned a home namespace,
and orthogonal to that different collections of mappings have to be collected.

  @dev project is open for work
  @src project is open for reading
  @current project is being accessed locally
  @working project is being run locally

The more classes, the more entities end up in the system.
