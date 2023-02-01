# OrthoDocs

[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=ggabbiani_orthodocs&metric=alert_status)](https://sonarcloud.io/project/overview?id=ggabbiani_orthodocs)

![Package output example](docs/package.png "Package output example")

OrthoDocs is an API documentation generator and static analysis tool for the OpenSCAD scripting language. For the list of the feature planned and their status see [Project status](#project-status).

## How to document

OrthoDocs is able to recognize all the public global functions, modules and variables found in the source tree, complete with parameters and their default values. Sources can be passed as files, directories or both. Each source file (here referred to as *package*) will produce a corresponding API document.

- [OrthoDocs basics](unit-tests/docu-styles/ortho/lib-nocomments/README.md) - documenting uncommented code
- [Using decorations](unit-tests/docu-styles/ortho/lib-defdecorations/README.md) - default behaviour
- [Using custom decorations](unit-tests/docu-styles/ortho/lib-custom-decorations/README.md) - customized decoration
- [Using comments](unit-tests/docu-styles/ortho/lib-nodecorations/README.md) - documenting existing code

## The command line

OrthoDocs is a CLI only utility, the basic invocation is:

    $ orthodocs --src-root «path to source-root directory» --doc-root «path to document-root» «list of files and/or directory relative to source-root»

OrthoDocs will search in all the input directories for any scad file, the resulting file list being merged with the files eventually passed as positionals and analized.
The analysis results will be used for producing a number of Markdown documents (one for each
source file analized).

Assuming the following directory tree:

    /home/
    └── giampa/
        └── example/
            └── sources/
                ├── 2d.scad
                ├── annotated.scad
                ├── bbox.scad
                ├── type_trait.scad
                └── uncommented.scad

the following command executed in the directory «/home/giampa»

    /home/giampa $ orthodocs --src-root example/sources --doc-root example/docs

will scan all the SCAD sources in the source-root and produce the consequent documents in the doc-root

    /home/
    └── giampa/
        └── example/
            ├── docs/
            │   ├── 2d.md
            │   ├── annotated.md
            │   ├── bbox.md
            │   ├── type_trait.md
            │   └── uncommented.md
            └── sources/
                ├── 2d.scad
                ├── annotated.scad
                ├── bbox.scad
                ├── type_trait.scad
                └── uncommented.scad

If we want to keep the same source-root **while documenting only a subset** of the present sources the following command

    /home/giampa $ orthodocs --src-root example/sources --doc-root example/docs annotated.scad uncommented.scad

will produce

    /home/
    └── giampa/
        └── example/
            ├── docs/
            │   ├── annotated.md
            │   └── uncommented.md
            └── sources/
                ├── 2d.scad
                ├── annotated.scad
                ├── bbox.scad
                ├── type_trait.scad
                └── uncommented.scad

### Full command options

For more complex tasks consult the full command option list:

| Option             | Description
| ------------------ | -----------
| -a,--admonitions   | **FLAG** - When this flag is enabled any admonition found in annotations will be enriched with a corresponding emoj
| -d,--doc-root      | **OPTION** - Specifies the target document tree root either as absolute or current directory relative path. It is a **mandatory directory** parameter and - if not existing - it will be automatically created.
| -g,--graphs        | **OPTION** - List of root relative directories where placing dependency graphs. Needs --src-root.
| -h,--help          | **FLAG** - Print the help message and exit
| -i,--ignore-prefix | **OPTION** - Ignore this prefix in the Table of Contents when sorting
| -p,--private       | **OPTION** - Every documentation item (variable, function or module) prefixed with this string will not be published.
| --pd,--pkg-deps    | **OPTION** - Set package dependecies representation by text list or by a dependency graph (possible value **text**/**graph**, default to **text**)
| -s,--src-root      | **OPTION** - Source tree root - either an absolute or current directory relative path. It is a **mandatory existing directory** parameter.
| sources            | **POSITIONAL** - source sub-trees and/or files -
| -t,--toc           | **FLAG** - Generate a Table of Contents in the document tree root either as absolute or «source tree root» relative path
| -V,--verbosity     | **FLAG** - Set the message log verbosity
| -v,--version       | **FLAG** - Display program version information and exit

## Project status

The list of features expected in the v1 release with their current status:

| Feature                   | Status
| ------------------------- | ------
| [admonitions](https://www.markdownguide.org/hacks/#admonitions)| ✔
| comment styles            | ✔
| cross-reference           | ✔
| dependency graphs         | ✔
| global values             | ✔
| source license detection  | ✔
| table of contents         | ✔

## Supported OSs

| OS                        | Status
| ------------------------- | ------
| Linux support             | ✔
| macOS support             | ✔
| Windows support           | ✔
