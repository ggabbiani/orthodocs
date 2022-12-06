/*
 * This file contains only private items and consequently produces an empty 
 * documentation file.
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
