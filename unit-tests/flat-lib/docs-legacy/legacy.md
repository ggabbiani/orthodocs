# package legacy

This package is commented with standard SCAD comments: in order to get proper documentation
orthodocs must be executed with --anno-prolog="".

It is an isolated package (i.e. it doesn't include nor use any other package),
as such it will not contain any dependecy list.

Copyright @ 2022, Giampiero Gabbiani (giampiero@gabbiani.org)

SPDX-License-Identifier: [GPL-3.0-or-later](https://spdx.org/licenses/GPL-3.0-or-later.html) or [MIT](https://spdx.org/licenses/MIT.html) WITH [Classpath-exception-2.0](https://spdx.org/licenses/Classpath-exception-2.0.html)


## Variables

---

### variable $legacy_global_public

__Default:__

    "global public $variable"

This $variable is global public and will be published.


---

### variable legacy_global_public

__Default:__

    "global public variable"

This variable is global public and will be published.


## Functions

---

### function legacy_global_public

__Syntax:__

```text
legacy_global_public(parm1=1,parm2=0)
```

This function is global public and will be published.


__Parameters:__

__parm1__  
This is the first parameter

__parm2__  
This is the second parameter


## Modules

---

### module legacy_global_public

__Syntax:__

    legacy_global_public(param1="default value for parm1",param2)

This module is global public and will be published.


