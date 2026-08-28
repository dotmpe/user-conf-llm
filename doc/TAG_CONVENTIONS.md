---
title: Tag conventions (+User-Conf-LLM edition)
status: idea
category: [ convention ]
audience: [ developer ]
tag: sketch extension
---

Tag are designated, decorated mnemonics for organizing "stuff": really any sort of entity.
The intent is to use them in modules, as user defined values,
for queries and rules. 
(Somewhat like options on a CLI, but with user defined aliases?)

Tag as a metadata value are very simple values currently.
No quoting or escaping, its exact format is set by DOC_CONVENTIONS

---

tag are important user notations, but without prior semantics 

see other notes on mnemonics and name (NAME_CONVENTIONS) usage

tag can treated as space separated 'tags',
but the free-form nature may make the bulk of them unusable for cross-indexing

---

When you tie tag very closely to a script you can build a sort of rule system.

The goal is here is a toolkit approach, to apply abstract ideas to existing parts of the system. 

Tag have to coordinate their language across project contexts,
ie. mnemonic usage through mapped name spaces.

For this all the user's tag and all the other source terms are assigned a home namespace,
and different collections of mappings have to be collected.

---

  @dev project is open for work
  @src project is open for reading
  @current project is being accessed locally
  @working project is being run locally

The more classes, the more entities end up in the system.
