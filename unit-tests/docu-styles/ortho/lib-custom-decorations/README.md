# Using custom decorations

It is possible to use any string/character as decoration for the annotations through the option `--decorations` like in the following examples:

```text
/*@
 * multi-line annotation
 * example.
 */
```

```text
//@ single-line annotation example
```

In this case OrthoDocs must be invoked with `--decorations` set to `"@"`.

As with the defaults, even with custom decorations any comment not matching and/or outside relevant positions will be ignored.
