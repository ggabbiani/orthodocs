/*!
 * this is a test package
 */

//! This is the *first* documented variable
global = [1,2,3];

//! a test module ... just for testing
module test(
  //! this parameter is CRITICAL
  par="ciao"
) {
  //! this is an ignored variable
  local =3;
}

//! this is the *second* documented variable
global_2 = let(p=3) 4 + p;

//! Identity matrix in homogeneous coordinate format
I=[
  [1,0,0,0],
  [0,1,0,0],
  [0,0,1,0],
  [0,0,0,1],
];