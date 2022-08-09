/*! test package */

glb1 = "I'm a global variable";

//! general 2d polygon bounding box
function fl_bb_polygon(
  //! points used for bounding box calculation
  points
) = let(
  x = [for(p=points) p.x],
  y = [for(p=points) p.y]
) [[min(x),min(y)],[max(x),max(y)]];

function pi() = PI;

function f(
  //! parameter P1
  p1,
  //! parameter P2
  p2=3,
  //! parameter P3
  p3=undef) = 0;

  module one() {
    loc1 = "I'm a local variable";
  }

  module two(
    //! first parameter
    p1 ,
    //! second parameter
    p2 = undef
  ) {
    module inner(
      //! inner module parameter
      p=undef
    ) {

    }
  }