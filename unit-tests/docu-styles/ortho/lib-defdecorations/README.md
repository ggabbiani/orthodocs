# Using default decorations

OrthoDocs __annotations are decorated comments__: multi or single line comments with the comment start immediately followed by a decoration string. The default is the character `'!'` like in the following examples:

```text
/*!
 * multi-line annotation
 * example.
 */
```

```text
//! single-line annotation example
```

Invoked without any `--decorations` specifications, OrthoDocs will use default annotations to enrich the documentation items, and will ignore any not matching comment eventually present in the source code.

__The principle is simple__: if you wish to document a module, add the annotation immediately before its definition, if you wish to document a formal parameter, place the annotation just before of it.

__Comments outside well-known positions will be ignored by OrthoDocs.__

| What to document  | Where to place comment                                          |
| ----------------- | --------------------------------------------------------------- |
| Function          | just before the function definition                             |
| Module            | just before the module definition                               |
| Package           | first comment in a SCAD source file before any other statement  |
| Formal parameter  | just before the formal parameter definition                     |
| Variable          | just before the variable definition                             |
|                   |                                                                 |

## Annotation internal format

Every annotation's content is analized in the SCAD domain with the referred item deduced by the annotation position. In relation to their internal format, this implies the following:

- no need for further tagging/markup (no @brief, @param and so on ...)
- the internal format is generally free even if:
  - is _loosely_ related to the source language in the expressions used for cross-reference
  - _should_ be neutral or at least compatible with respect to the documentation output format (currently __Markdown only__).
