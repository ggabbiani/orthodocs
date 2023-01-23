/*
 * This package is not commented at all.
 * It is an isolated package (i.e. it doesn't include nor use any other package),
 * as such it will not contain any dependecy list.
 *
 * Copyright @ 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later or MIT WITH Classpath-exception-2.0
 */

__undocumented_global_private__ = 10;

undocumented_global_public = "global public variable";

$undocumented_global_public = "global public $variable";

function __undocumented_global_private__(param1,param2="default value") = false;

function undocumented_global_public(
  parm1=1,
  parm2=0
) = true;

module undocumented_global_public(param1="default value for parm1",param2) {
  function undocumented_nested_function(
    param1,
    param2="default value for parm2"
  ) = false;

  module undocumented_nested_module(
    param1,
    param2,
    param3="default value for parm3"
  ) {
  }

}
