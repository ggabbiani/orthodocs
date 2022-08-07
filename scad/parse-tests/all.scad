/*
 This is
 a file comment
 */

include <2d.scad>
include <bbox.scad>
include <type_trait.scad>

// this comment refers to use <file3.scad>
use     <file3.scad>
use     <part4/file4.scad>

// skipped #1

// skipped #2

/* 'annotated' identifier */
identifier1 = true;

uncommented = undef;

int_1 = 1;
int_2 = -1;
int_3 = +1;

fp_1    = 0.0;  // Floating-point zero in default format
fp_2    = 0E0;  // Also floating-point zero in default format
fp_3    = -.0;  // This may be a zero or a minus zero, depending on the implementation
fp_4    = 0.;   // The integer zero, not a floating-point zero!
fp_5    = 3.1415926535897932384;    // A double-format approximation to π
fp_6    = 6.02E+23; // Avogadro's number, in default format
fp_7    = 602E+21;  // Also Avogadro's number, in default format

include <file5.scad>

str_1   = "The quick brown fox \tjumps \"over\" the lazy dog.\rThe quick brown fox.\nThe \\lazy\\ dog.";
str_2   = "\u03a9";
// echo(str_2=str_2);

// lookup test
lookup  = int_2;

// expressions
int_4 = int_2 * 3;
boolean = int_3==int_1;

not_expression = !boolean;

ternary = not_expression!=false ? 1 : 2;

vector=[0,1,2];
val1 = vector[2];
val2 = let(i=4) vector[i - 1];

// let clause
v = let(a=2,b="string") a;

// function calls
fc_1 = abs(-3);

//! function definition
function func_1(p1,p2) = let(
  p = p1 + p2
) p * p;

//! this module has a special character
module $mod_1(p1,p2) {
  var = func_1(1,2);
  children();
  pippo() {}
  pluto() {
    first(2,"ciao");
    second(0);
  }
}

$mod_1();
