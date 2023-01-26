/*
 * Copyright @ 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: MIT or GPL-3.0-or-later
 */
include <referred.scad>

/*
 * This variable is global private and will not be published.
 * See also variable __referred_global_private__.
 */
__referring_global_private__ = 10;

/*
 * This variable is global public and will be published.
 * See also variable referred_global_public.
 */
referring_global_public = "global public variable";

/*
 * This \$variable is global public and will be published.
 * See also variable $referred_global_public.
 */
$referring_global_public = "global public $variable";

/*
 * This function is global private and will not be published.
 * See also function __referred_global_private__().
 */
function __referring_global_private__(param1,param2="default value") = false;

/*
 * This function is global public and will be published.
 * See also function referred_global_public().
 */
function referring_global_public(
  parm1=1,  // This is the first parameter
  parm2=0   // This is the second parameter
) = true;

/*
 * This module is global public and will be published.
 * See also module referred_global_public{}.
 */
module referring_global_public(param1="default value for parm1",param2) {
  /*
   * This function is nested and will not be published.
   * See also function referred_nested_function().
   */
  function referring_nested_function(
    param1,
    param2="default value for parm2"
  ) = false;

  /*
   * This module is nested and will not be published.
   * See also module referred_nested_module{}.
   */
  module referring_nested_module(
    param1,
    param2,
    param3="default value for parm3"
  ) {
  }

}
