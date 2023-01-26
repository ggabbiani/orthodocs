# Using default decorated comments as annotation

The default decoration used by OrthoDocs is the single character `'!'` soon after
any native SCAD comment start.

The option `--anno-prolog` can be left unset.

Valid examples for annotations are:

```
/*!
 * multi-line annotation
 * example.
 */
```

```
//! single-line annotation example
```

Comments without the default decoration or with different decorations will be ignored.