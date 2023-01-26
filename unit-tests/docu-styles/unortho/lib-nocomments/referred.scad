__referred_global_private__ = 10;

referred_global_public = "global public variable";

$referred_global_public = "global public $variable";

function __referred_global_private__(param1,param2="default value") = false;

function referred_global_public(
  parm1=1,
  parm2=0
) = true;

module referred_global_public(
  param1="default value for parm1",
  param2) {
  function referred_nested_function(
    param1,
    param2="default value for parm2"
  ) = false;

  module referred_nested_module(
    param1,
    param2,
    param3="default value for parm3"
  ) {
  }

}
