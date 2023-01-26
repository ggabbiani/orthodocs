include <referred.scad>

__referring_global_private__ = 10;

referring_global_public = "global public variable";

$referring_global_public = "global public $variable";

function __referring_global_private__(param1,param2="default value") = false;

function referring_global_public(
  parm1=1,
  parm2=0
) = true;

module referring_global_public(param1="default value for parm1",param2) {
  function referring_nested_function(
    param1,
    param2="default value for parm2"
  ) = false;

  module referring_nested_module(
    param1,
    param2,
    param3="default value for parm3"
  ) {
  }

}
