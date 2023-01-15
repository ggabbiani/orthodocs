/*!
 * This file contains nested function and module that should be not published.
 *
 * Copyright @ 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
