---
title: Document conventions
status: draft
category: [ convention ]
audience: [ developer ]
tag: Markdown YAML metadata
---
User-Tools uses a single format for documentation and other structured plain text files.

## Intro

This document describes document usage conventions, it defines the format, the metadata format, and its schema and vocabulary.

User-Tools currently has a single documentation repository, appendix III explains its the current conventions.

## Format

All documents are Markdown files and may be free-form;
and optionally include a metadata block.
The metadata is currently the only means to differentiate file kind, including its notation conventions.

Future conventions may further restrict this, in particular the use of soft and
hard line breaks, and the exact formatting and notations allowed or required for releases.

## Metadata format

Alternative to a title, more explicit metadata for Markdown documents can be specified in the front matter, as YAML (subset) block of keys and values.

The only allowed value ranges are:

- plain scalars;
- inlined YAML lists, of only plain scalars.

No quoting, no special characters, no YAML literals. That is all. No further examination is done, that requires concrete testing and a use case (first).

### Syntax summary

- One block per document, marked by literal sentinel `---` line before and after.

- Keys are unique, use lower-case ASCII with hyphens or underscores, and appear once only.

- Values do not continue across lines.

- No nested structures or other YAML syntax.

- No quotes, no reserved special or meta characters.

Values should comply to YAML parsing rules to be valid. And without any quoting or escaping (see `Future extensions`). For purposes of +User-Tools it is constrained by the expression given in `Parsing guidelines`, and that is sufficient. (But it may change at new versions)

Future extensions should consider what level of escape/quoting can be supported, and what makes sense given the pre-established context.

Forbidden literal YAML values:

- leading `-`: sequence item;
- leading `{` or `[`, those are reserved for collections;
- leading `---` or `....`

### Parsing guidelines

- Plain scalars are the single-line value after the first `:`, trimmed at both ends.

- The types are fixed per schema (next section) assigned to the key;
  plain scalar type fields are never interpret as list, and can contain comma;
  list type fields must always be formatted as a list and can never contain comma.

- All plain scalar are interpreted as free form text ie. strings; there are no numbers, booleans or null values.

- Only plain scalar can have blank space, and they should collapse adjacent blank characters.

- For lists, blank space around syntax and adjacent to scalars should be ignored completely.

- Neither type of scalar value can start with `-` `{` `[`

For the current (dev) version the format and ranges are: `[A-Za-z0-9][A-Za-z0-9_ ().,/@+-]*`.

This may change at any point to a more definite (restricted or relaxed) range,
and it may be better specified as a forbidden range in a future version.

### Future extensions

See the appendix for considerations to add quoting and support more characters in the metadata.

## Metadata schema

When specified, the metadata block must have a title.

The document should not provide a top-level title, but continue the root level text and start new sections at the second level.

### Uncontrolled vocabularies

**Title**, **Subtitle**: the title and optional subtitle.

**Tag**: a space separate sequence of tag (unquoted). 
See [Tag conventions](TAG_CONVENTIONS.md).

### Controlled vocabularies

**Status**: Document life cycle and track

- idea
- draft
- review
- proposal
- proposed
- accepted
- stable
- deprecated
- superseded
- archived

**Categories**: Collections for overall type, format and/or subject(s)

- convention
- reference
- design
- schema
- guide
- workflow
- policy
- architecture
- index

**Audience**: Collections for document types or included parts that are specific to roles and tasks

- developer
- maintainer
- user

The end user may not own development environments, and still not only just run but also **write** +User-Tools scripts using some distribution surface. Only any script writer wanting to process source becomes a developer--but may be not a co-developer.

A maintainers focus should be on state or quality and shippable items: the package, and its prerequisites (ie. versions and releases). The documentation should cover distribution and configuration interface, and only in so far as typical distribution pipeline need, and for those which there are output channels (source, docs, extra).

## Concluding remarks

These are current values, good for next dev release.

(v0.0.2)

TODO: other organisation axis can be added, if the markdown YAML frontmatter turns out as a suitable location for such data (later dev versions). See Appendices.

## Appendix I: Additional metadata organisations

### Overall organisation

- **Domain**
- **Project**

Default, implied value for project would be `all`.

### Project stage, environment and other related organisation

- **Layer**
- **Scope**
- **Profile**

### Outline documents and other styles, standards
For skeletons use outlines with additional metadata fields:

- **Profile**: `outline`
- **Strictness**: describe how outline can be used;
  Values: `suggested`, `recommended`, `required`

To apply outline to documents:

- **Outline**: a named outline that is adhered to by document.
  Default: `none` (free form section outline)

For other styleguides and standard references included in body apply:

- **References**: list external standard ID's, from other standards bodies

## Appendix II: Extended metadata range

With single and double quoted scalars come several requirements.

```
document = metadata-block
metadata-block = "---" newline fields "---"
field = key ":" value
value = plain-scalar
       | single-quoted-scalar
       | double-quoted-scalar
       | inline-list
inline-list = "[" [scalar *(comma scalar)] "]"
```

## Appendix III: Documentation repository usage

The documentation is in
the ``doc/`` of the +User-Tools repository.

For convenience all projects
in the suite set their ``doc``

