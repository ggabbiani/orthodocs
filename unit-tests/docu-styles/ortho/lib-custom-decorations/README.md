# Using custom decorated comments as annotation

It is possible to use any string/character as decoration for annotations.

The option `--decorations` must be used for this.

Valid examples for custom annotations are:

```
/*@
 * multi-line annotation
 * example.
 */
```

```
//@ single-line annotation example
```

Comments without the default decoration or with different decorations will be ignored.