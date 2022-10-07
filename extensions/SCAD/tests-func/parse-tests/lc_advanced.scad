/*
 * list comprehension: Advanced Examples
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
 * along with ADOX.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * GENERATING VERTICES FOR A POLYGON
 * Using list comprehension, a parametric equation can be calculated at a
 * number of points to approximate many curves, such as the following example
 * for an ellipse (using polygon()):
 */
sma = 20;  // semi-minor axis
smb = 30;  // semi-major axis

polygon(
    [ for (a = [0 : 5 : 359]) [ sma * sin(a), smb * cos(a) ] ]
);

/*
 * FLATTENING A NESTED VECTOR
 * List comprehension can be used in a user-defined function to perform tasks
 * on or for vectors. Here is a user-defined function that flattens a nested
 * vector.
 */

// input : nested list
// output : list with the outer level nesting removed
function flatten(l) = [ for (a = l) for (b = a) b ] ;

nested_list = [ [ 1, 2, 3 ], [ 4, 5, 6 ] ];
echo(flatten(nested_list)); // ECHO: [1, 2, 3, 4, 5, 6]

/*
 * SORTING A VECTOR
 * Even a complicated algorithm Quicksort becomes doable with for(), if(),
 * let() and recursion
 */

// input : list of numbers
// output : sorted list of numbers
function quicksort(arr) = !(len(arr)>0) ? [] : let(
    pivot   = arr[floor(len(arr)/2)],
    lesser  = [ for (y = arr) if (y  < pivot) y ],
    equal   = [ for (y = arr) if (y == pivot) y ],
    greater = [ for (y = arr) if (y  > pivot) y ]
) concat(
    quicksort(lesser), equal, quicksort(greater)
);

// use seed in rands() to get reproducible results
unsorted = [for (a = rands(0, 10, 6, 3)) ceil(a)];
echo(unsorted); // ECHO: [6, 1, 8, 9, 3, 2]
echo(quicksort(unsorted)); // ECHO: [1, 2, 3, 6, 8, 9]

/*
 * SELECTING ELEMENTS OF A VECTOR
 * select() performs selection and reordering of elements into a new vector.
 */

function select(vector, indices) = [ for (index = indices) vector[index] ];

vector1 =   [[0,0],[1,1],[2,2],[3,3],[4,4]];
selector1 = [4,0,3];
vector2 =   select(vector1,selector1);    // [[4, 4], [0, 0], [3, 3]]
vector3 =   select(vector1,[0,2,4,4,2,0]);// [[0, 0], [2, 2], [4, 4],[4, 4], [2, 2], [0, 0]]
// range also works as indices
vector4 =   select(vector1, [4:-1:0]);    // [[4, 4], [3, 3], [2, 2], [1, 1], [0, 0]]

/*
 * CONCATENATING TWO VECTORS
 */

// Using indices
function cat(L1, L2) = [for (i=[0:len(L1)+len(L2)-1])
                        i < len(L1)? L1[i] : L2[i-len(L1)]] ;
echo(cat([1,2,3],[4,5])); //concatenates two OpenSCAD lists [1,2,3] and [4,5], giving [1, 2, 3, 4, 5]

// Without using indices:

function cat(L1, L2) = [for(L=[L1, L2], a=L) a];
echo(cat([1,2,3],[4,5])); //concatenates two OpenSCAD lists [1,2,3] and [4,5], giving [1, 2, 3, 4, 5]

function fl_push(list,item) = [each list,item];
A = [each list,a,[]];
