/*!
 * This file is not depending on enything else, so no dependency graph or 
 * list will be shown.
 *
 * Copyright @ 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later or MIT
 */

/*!
 * private variable not published even if annotated.
 */
__var__ = 10;

/*!
 * this variable will be published.
 */
PREFIX_PUB_VAR = "public variable";

/*!
 * this $variable will be published.
 */
$PREFIX_PUB_VAR = "$public variable";

/*!
 * this variable must demonstrate that the Table Of Contents sort is case insensitive.
 */
prefix_pub_var = "public lower case variable";

/*!
 * private func not published even if annotated.
 */
function __func__(param1,param2="default value") = false;

/*!
 * this function will be published.
 */
function prefix_radial(
  // this parameter will be NOT published
  r=1,
  //! this parameter will be published
  theta=0
) = 3.14;
