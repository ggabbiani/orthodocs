# package foundation/3d

## Dependencies

```mermaid
graph LR
    A1[foundation/3d] --o|include| A2[foundation/2d]
    A1 --o|include| A3[foundation/bbox]
    A1 --o|include| A4[foundation/type_trait]
```

## Variables

---

### variable e

__Default:__

    2.71828

## Functions

---

### function fl_3d_AxisList

__Syntax:__

```text
fl_3d_AxisList(axes)
```

---

### function fl_3d_axisIsSet

__Syntax:__

```text
fl_3d_axisIsSet(axis,list)
```

## Modules

---

### module fl_cube

__Syntax:__

    fl_cube(verbs=FL_ADD,size=[1,1,1],octant,direction)

---

### module fl_cylinder

__Syntax:__

    fl_cylinder(verbs=FL_ADD,h,r,r1,r2,d,d1,d2,octant,direction)
