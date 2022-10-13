/*!
 * This file contains nested function and module that should be not published.
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

include <isolated.scad>
include <undocumented.scad>

/*!
 * public func to be published.
 */
function prefix_global_func(param1,param2="default value") = false;

/*!
 * public module to be published.
 */
module prefix_global_module(param1="default value for parm1",param2) {
  /*!
   * nested func to be ignored.
   */
  function nested_func(param1,param2="default value for parm1") = false;

  /*!
   * nested module to be ignored.
   */
  module nested_module(param1,param2,param3="default value for parm3") {

  }

}
