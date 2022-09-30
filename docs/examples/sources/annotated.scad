/*
 * Annotated SCAD example file.
 *
 * Copyright © 2022 Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * ODOX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ODOX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ODOX.  If not, see <http://www.gnu.org/licenses/>.
 */

/*!
 * 3d primitives replacing native OpenSCAD ones.
 */

include <2d.scad>
include <bbox.scad>
include <type_trait.scad>

/*!
 * Build a floating semi-axis list from literal semi-axis list.
 *
 * example:
 *
 *     list = fl_3d_AxisList(axes=["-x","±Z"]);
 *
 * is equivalent to:
 *
 *     list =
 *     [
 *      [-1, 0,  0],
 *      [ 0, 0, -1],
 *      [ 0, 0, +1],
 *     ];
 *
 * __note:__ this is an admonition!
 */
function fl_3d_AxisList(
  //! semi-axis list (es.["-x","±Z"])
  axes
) = [];

//! wether «axis» is present in floating semi-axis list
function fl_3d_axisIsSet(axis,list) = false ;

/*!
 * cube replacement
 */
module fl_cube(
  //! FL_ADD,FL_AXES,FL_BBOX
  verbs     = FL_ADD,
  size      = [1,1,1],
  //! when undef native positioning is used
  octant,
  //! desired direction [director,rotation] or native direction if undef
  direction
) {

}

/*!
 * The number e, also known as Euler's number, is a mathematical
 * constant approximately equal to 2.71828 which can be characterized
 * in many ways.
 */
e = 2.71828;

/*!
 * cylinder replacement
 */
module fl_cylinder(
  //! FL_ADD,FL_AXES,FL_BBOX
  verbs  = FL_ADD,
  //! height of the cylinder or cone
  h,
  //! radius of cylinder. r1 = r2 = r.
  r,
  //! radius, bottom of cone.
  r1,
  //! radius, top of cone.
  r2,
  //! diameter of cylinder. r1 = r2 = d / 2.
  d,
  //! diameter, bottom of cone. r1 = d1 / 2.
  d1,
  //! diameter, top of cone. r2 = d2 / 2.
  d2,
  //! when undef native positioning is used
  octant,
  //! desired direction [director,rotation], native direction when undef ([+X+Y+Z])
  direction
) {

}