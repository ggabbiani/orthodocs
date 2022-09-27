/*!
 * This file is not depending on enything else, so no dependency graph or 
 * list will be shown.
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
 * private variable not published even if annotated.
 */
__var__ = 10;

/*!
 * this variable will be published.
 */
PUB_VAR = "public variable";

/*!
 * this variable must demonstrate that the Table Of Contents sort is case insensitive.
 */
pab_var = "public lower case variable";

/*!
 * private func not published even if annotated.
 */
function __func__(param1,param2="default value") = false;

/*!
 * this function will be published.
 */
function radial(
  // this parameter will be NOT published
  r=1,
  //! this parameter will be published
  theta=0
) = 3.14;
