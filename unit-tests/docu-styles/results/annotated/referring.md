# package referring

## Dependencies

```mermaid
graph LR
    A1[referring] --o|include| A2[referred]
```

Copyright @ 2022, Giampiero Gabbiani (giampiero@gabbiani.org)

SPDX-License-Identifier: [MIT](https://spdx.org/licenses/MIT.html) or [GPL-3.0-or-later](https://spdx.org/licenses/GPL-3.0-or-later.html)


## Variables

---

### variable $referring_global_public

__Default:__

    "global public $variable"

This \$variable is global public and will be published.
See also variable $referred_global_public.


---

### variable referring_global_public

__Default:__

    "global public variable"

This variable is global public and will be published.
See also [variable referred_global_public](referred.md#variable-referred_global_public).


## Functions

---

### function referring_global_public

__Syntax:__

```text
referring_global_public(parm1=1,parm2=0)
```

This function is global public and will be published.
See also function [referred_global_public()](referred.md#function-referred_global_public).


__Parameters:__

__parm1__  
This is the first parameter

__parm2__  
This is the second parameter


## Modules

---

### module referring_global_public

__Syntax:__

    referring_global_public(param1="default value for parm1",param2)

This module is global public and will be published.
See also module [referred_global_public{}](referred.md#module-referred_global_public).


