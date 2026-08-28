---
title: Document conventions
status: draft
category: [ convention ]
audience: [ developer ]
tag: Markdown YAML metadata
---

Markdown files may be free-form;
or optionally include a metadata block. 

This defines the metadata format, its schema keys and vocabulary.

## Metadata format

Alternative to a title more explicit metadata for Markdown documents can be specified in the front matter,
as YAML (subset) block.

The only allowed value ranges are:

- plain scalars;
- inlined YAML lists, of only plain scalars.

The types are fixed per schema (next section);
plain scalar type fields are never interpret as list,
and list type fields must always be formatted as a list.

Plain scalars are the single-line value after the first `:`, trimmed at both ends.
They must not contain a colon followed by whitespace or a ` #` sequence.

All scalar are interpreted as free form text ie. strings; there are no numbers, booleans or null values.
The plain scalar is the trimmed remainder of the line, with whitespace collapsed to space;
for lists, white space is ignored completely including scalars (no spaces in vocabulary terms).

See the appendix for considerations to add quoting and support more characters in the metadata.

For the current (dev) version the format and ranges are: `[A-Za-z0-9][A-Za-z0-9_ ().,/@+-]*`.

Forbidden literal YAML:

- leading `-`: sequence item;
- leading `{` or `[` if those are reserved for collections;
- leading `---` or `....`

Additional and summary points:

- One block per document, marked by sentinel `---` line before and after.

- Keys are unique, use lower-case ASCII with hyphens or underscores, and appear once only.

- Values do not continue across lines.

- No nested structures or other YAML syntax.

## Metadata schema

When specified, the metadata block must have a title.
And the document itself does not give a top-level title.

### Uncontrolled vocabularies

**Title**, **Subtitle**: the title and optional subtitle.

**Tag**: a space separate sequence of tag (unquoted). 
See [Tag conventions](TAG_CONVENTIONS.md).

### Controlled vocabularies

**Status**:

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

**Categories**:

- convention
- reference
- design
- schema
- guide
- workflow
- policy
- architecture
- index

Audience

- developer
- maintainer

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
