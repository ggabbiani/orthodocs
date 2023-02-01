# OrthoDocs basics

The OrthoDocs documentation process can be divided in two phases:

- **SCAD analysis**: producing internal analitic documents from sources
- **Markdown synthesis**: translating internal analitic documents in the desired output format.

During the SCAD analysis, OrthoDocs uses an internal lexer/parser that separates comments from code, produces the AST and gathers its information into the analitic document. Comments and annotation are not discarded, but used to enrich already present information, basically **you don't need comments to document your sources**.

The SCAD items always documented are:

| Item              | Notes                                             |
| ----------------- | ------------------------------------------------- |
| Formal parameters | in function or module scope                       |
| Functions         | in global scope                                   |
| Modules           | in global scope                                   |
| Package           | all the public accessible APIs in a SCAD source   |
| Variables         | in global scope                                   |
|                   |                                                   |

The availability matrix for the supported features without and with comments is the following:

| Type      | Feature                                       | Uncommented   | Commented |
| ---       | ---                                           | ---           | ---       |
| Packages  | direct dependecy list (textual or graphical)  | ✔            | ✔        |
|           | inter-package dependecy graph(s)              | ✔            | ✔        |
|           | detailed description                          | ❌            | ✔        |
|           | SPDX licensing                                | ❌            | ✔        |
| Variables | signature (name and defaults)                 | ✔            | ✔        |
|           | detailed description                          | ❌            | ✔        |
| Functions | signature (name, prameters and defaults)      | ✔            | ✔        |
|           | detailed description                          | ❌            | ✔        |
|           | parameter detailed description                | ❌            | ✔        |
| Modules   | signature (name, prameters and defaults)      | ✔            | ✔        |
|           | detailed description                          | ❌            | ✔        |
|           | parameter detailed description                | ❌            | ✔        |
| General   | Table of Contents                             | ✔             | ✔        |
|           | Cross references                              | ❌            | ✔        |
|           | Admonitions                                   | ❌            | ✔        |
|           |                                               |               |           |

## Restricting the produced documentation

Currently SCAD has no general namespacing mechanism to mark a global API as public or private, every global API is always public accessible and consequently always documented by OrthoDocs.

This behaviour however, produces undesired documentation for global APIs used inside package and/or library but not meant to be exported.

Using a consistent API naming convention it is possible - with the `--private` option - to inhibit the documentation for the items considered 'private'.

If - for example - the programmer decides to prefix private API elements with double underscores (`__`) an OrthoDocs invokation with `--private "__"` will prevent every function, module or variable so prefixed from being documented (this is not implemented for formal parameters __yet__).
