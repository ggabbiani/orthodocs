/*
 * This file contains only private items and consequently produces an empty 
 * documentation file.
 *
 * Copyright @ 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

include <isolated.scad>

/*!
 * private variable not included
 */
__var__ = 10;

/*!
 * private function not included 
 */
function __func__(param1,param2="default") = 10;

/*!
 * private module not included 
 */
module __module__(param1,param2="default") {
  echo(str("Variable = ", __var__));
}

__module__(1,"Hi");
