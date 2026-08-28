---
title: Naming conventions
status: draft
category: [ convention ]
audience: [ developer ]
tag:
---

This document provides detailed naming conventions and considerations.

## Name formats

Name format should be controlled. Some special considerations:

* Names generally conform to
 
  - **strict-symbol**: ``[A-Z_][A-Z0-9_]*``
  - **function-name**: ``[a-z_][a-z0-9_]*``
  - **resource-name**: ``[A-Za-z0-9][A-Za-z0-9-]+``

  These contain individual words or tag, concatenated by a non-word character.

  The resource-name format is often the preferred one for system readable files,
  and others two for exposed system internals and user definitions.

  - **fragment-name**: implementation specific

- Periods are already overloaded with meanings; it's a good general purpose
  (blank) space alternative as well (alternatively/in addition to hyphen or underscore); 
  but depending on the context.

- Use . / ~ otherwise with the conservative meaning(s), only to be amended for
  new contexts where appropriate.

* The same goes for many special ASCII characters; their usefulness is as
  decorative mnemonic but all too many combinations will overload that.

  Recognition is a factor: the smaller a presence is, the harder it might be to spot or find.
  Consider defining a real mnemonic alpha(numeric) tag as the definite symbol,
  and to associate that for a certain decoration by some input(s).

- Dotfiles are conventional hidden files;
  file name extensions follow a base name with period concatenated tags representing file format, encoding and envelopes.

  Both infer a base name without periods; two 

  - for hidden files, consider whether the "unhidden"/unprefixed file can exist.

    Normally the user should be allowed to choose for either,
    but so then the names must be kept exclusive.

  - for shared bases (file names with different extensions sharing a prefix),
    an exact match before the first period indicates the same name.

    If relevant, considered whether and which different extensions can exist simultaneously.


### Name overrides, extensions, stand-ins, etc.

I want to experiment with two generic patterns:

- Underscore prefixing as a form of local override.

  The underscore prefixed name can be considered to be primary if it exists,
  overriding the normal name.

- Suffix tagging, use a special like . - + to append at the base itself new tag.

  This is in particular useful to establish parallel versions for files, and so to
  keep clean, separate histories (for direct, frictionless merge/rebase in SCM workflows).

[tbd: also see ``configure+skeleton.bash`` script & relevant documentation]

Can use prefix/suffix on local base name as well as any name element in path,
to mark insertions or special paths, 
to be picked up on by lookup path configuration scripts or rules.

So for user-tools/std template config, fix on:

`./{,.}{,_}local/` or `~/{,.}{,_}local` or `/usr/local/<deploy-tag>` 

- do not want to use config, conf. those collide easily, keep those as
  tag/sub

- Also tool/.... is heavily used currently:

    - tool/<suite>/<tag>/*.*
    - tool/<lang>/<tag>/*.*


## Conclusion
