/*@
 * This is an isolated package (i.e. it doesn't include nor use any other package),
 * as such it will not contain any dependecy list.
 *
 * Copyright @ 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later or MIT WITH Classpath-exception-2.0
 */

/*@
 * This variable is global private and will not be published.
 */
__referred_global_private__ = 10;

/*@
 * This variable is global public and will be published.
 */
referred_global_public = "global public variable";

/*@
 * This $variable is global public and will be published.
 */
$referred_global_public = "global public $variable";

/*@
 * This function is global private and will not be published.
 */
function __referred_global_private__(param1,param2="default value") = false;

/*@
 * This function is global public and will be published.
 */
function referred_global_public(
  parm1=1,  //@ This is the first parameter
  parm2=0   //@ This is the second parameter
) = true;

/*@
 * This module is global public and will be published.
 */
module referred_global_public(
  param1="default value for parm1", //@ this is a single line documentation for param1
  param2
  /*@
   * this is a multiline
   *
   * documentation for param2
   */
  ) {
  /*@
   * This function is nested and will not be published.
   */
  function referred_nested_function(
    param1,
    param2="default value for parm2"
  ) = false;

  /*@
   * This module is nested and will not be published.
   */
  module referred_nested_module(
    param1,
    param2,
    param3="default value for parm3"
  ) {
  }

}
