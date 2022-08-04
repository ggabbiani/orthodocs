/*
 * list comprehension: for
 *
 * Copyright © 2022 Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * This file is part of the 'AutoDox' (ADOX) project.
 *
 * ADOX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ADOX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ADOX.  If not, see <http: //www.gnu.org/licenses/>.
 */


/*
 * FIRST VARIANT: [ for (i = [start : step : end]) i ]
 * Generate output based on a range definition, this version is mainly useful
 * to calculate list values or access existing lists using the range value as
 * index.
 */

// generate a list with all values defined by a range
list1 = [ for (i = [0 : 2 : 10]) i ];
echo(list1); // ECHO: [0, 2, 4, 6, 8, 10]

// extract every second character of a string
str = "SomeText";
list2 = [ for (i = [0 : 2 : len(str) - 1]) str[i] ];
echo(list2); // ECHO: ["S", "m", "T", "x"]

// indexed list access, using function to map input values to output values
function func(x) = x < 1 ? 0 : x + func(x - 1);
input = [1, 3, 5, 8];
output = [for (a = [ 0 : len(input) - 1 ]) func(input[a]) ];
echo(output); // ECHO: [1, 6, 15, 36]

/*
 * SECOND VARIANT: [ for (i = [a, b, c, ...]) i ]
 * Use list parameter as input, this version can be used to map input values to
 * calculated output values.
 */

// iterate over an existing list
friends = ["John", "Mary", "Alice", "Bob"];
list = [ for (i = friends) len(i)];
echo(list); // ECHO: [4, 4, 5, 3]

// map input list to output list
list = [ for (i = [2, 3, 5, 7, 11]) i * i ];
echo(list); // ECHO: [4, 9, 25, 49, 121]

// calculate Fibonacci numbers
function func(x) = x < 3 ? 1 : func(x - 1) + func(x - 2);
input = [7, 10, 12];
output = [for (a = input) func(a) ];
echo(output); // ECHO: [13, 55, 144]

/*
 * THIRD VARIANT: [ for (c = "String") c ]
 * Generate output based on a string, this iterates over each character of the
 * string.
 */

echo([ for (c = "String") c ]);
// ECHO: ["S", "t", "r", "i", "n", "g"]

/*
 * FOURTH VARIANT: [ for (a = inita, b = initb, ...;condition;a = nexta, b = nextb, ...) expr ]
 * Generator for expressing simple recursive call in a c-style for loop.
 * Note: from OpenSCAD version 2019.05
 */

echo( [for (a = 0, b = 1;a < 5;a = a + 1, b = b + 2) [ a, b * b ] ] );
// ECHO: [[0, 1], [1, 9], [2, 25], [3, 49], [4, 81]]

// Generate fibonacci sequence
echo([for (a = 0, b = 1;a < 1000;x = a + b, a = b, b = x) a]);
// ECHO: [0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987]

// Cumulative sum of values in v
function cumsum(v) = [for (a = v[0]-v[0], i = 0; i < len(v); a = a+v[i], i = i+1) a+v[i]];
echo(cumsum([1, 2, 3, 4]));
// ECHO: [1, 3, 6, 10]
echo(cumsum([[1, 1], [2, 2], [3, 3]]));
// ECHO: [[1, 1], [3, 3], [6, 6]]

/*
 * FIFTH VARIANT: each
 * each embeds the values of a list given as argument directly, effectively
 * unwrapping the argument list.
 * Note: from  OpenSCAD version 2019.05
 */

// Without using "each", a nested list is generated
echo([ for (a = [1 : 4]) [a, a * a] ]);
// ECHO: [[1, 1], [2, 4], [3, 9], [4, 16]]

// Adding "each" unwraps the inner list, producing a flat list as result
echo([ for (a = [1 : 4]) each [a, a * a] ]);
// ECHO: [1, 1, 2, 4, 3, 9, 4, 16]

/*
 * SIXTH VARIANT: [ for (i = list) if (condition(i)) i ]
 * When the evaluation of the condition returns true, the expression i is added to the result list.
 * The if element allows selection if the expression should be allocated and
 * added to the result list or not. In the simplest case this allows filtering
 * of an list.
 */

list = [ for (a = [ 1 : 8 ]) if (a % 2 == 0) a ];
echo(list); // ECHO: [2, 4, 6, 8]

// Note that the if element cannot be inside an expression, it should be at the top.

// from the input list include all positive odd numbers
// and also all even number divided by 2

list = [-10:5];
echo([for(n=list) if(n%2==0 || n>=0) n%2==0 ? n/2 : n ]);
// ECHO: [-5, -4, -3, -2, -1, 0, 1, 1, 3, 2, 5]
// echo([for(n=list) n%2==0 ? n/2 : if(n>=0) n ]); // this would generate a syntactical error

/*
 * SEVENTH VARIANT: [ for (i = list) if (condition(i)) x else y ]
 * When the evaluation of the condition returns true, the expression x is added to the result list else the expression y.
 * Note: from OpenSCAD version 2019.05
 * The if-else construct is equivalent to the conditional expression ?: except
 * that it can be combined with filter if.
 */

// even numbers are halved, positive odd numbers are preserved, negative odd numbers are eliminated
// echo([for (a = [-3:5]) if (a % 2 == 0) [a, a/2] else if (a > 0) [a, a] ]);
// ECHO: [[-2, -1], [0, 0], [1, 1], [2, 1], [3, 3], [4, 2], [5, 5]];

// // Note that in the expression above the conditional operator could not substitute if-else. It is possible to express this same filter with the conditional operator but with a more cryptic logic:

// // even numbers are halved, positive odd numbers are preserved, negative odd numbers are eliminated
// echo([for (a = [-3:5]) if (a % 2 == 0 || (a % 2 != 0 && a > 0)) a % 2 == 0 ? [a, a / 2] : [a, a] ]);
// // ECHO: [[-2, -1], [0, 0], [1, 1], [2, 1], [3, 3], [4, 2], [5, 5]];

// // To bind an else expression to a specific if, it's possible to use parenthesis.

// // even numbers are dropped, multiples of 4 are substituted by -1
// echo([for(i=[0:10]) if(i%2==0) (if(i%4==0) -1 ) else i]);
// // ECHO: [-1, 1, 3, -1, 5, 7, -1, 9]

// // odd numbers are dropped, multiples of 4 are substituted by -1
// echo([for(i=[0:10]) if(i%2==0) if(i%4==0) -1 else i]);
// // ECHO: [-1, 2, -1, 6, -1, 10]
