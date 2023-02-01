# Using comments

When invoked with empty decorations (`--decorations ""`) OrthoDocs consider every comment in well-known positions as annotation bound to the syntactic part of interest.

In the following example:

- the multiline comment preceeding the function definition is bound to the function detailed description
- comments immediately preceeding parameters are bound to the corresponding parameter's detailed description
- every other comment is ignored.

```scad
/*
 * This global public function
 * will be documented.
 */
function example_func(
  // This is the first parameter
  parm1=1,
  // This is the second parameter
  parm2=0
) =
  // this comment is ignored
  true;
```

The produced documentation is the following:

---

### function example_func

__Syntax:__

    example_func(parm1=1,parm2=0)

This global public function
will be documented.

__Parameters:__

__parm1__
This is the first parameter

__parm2__
This is the second parameter

---
