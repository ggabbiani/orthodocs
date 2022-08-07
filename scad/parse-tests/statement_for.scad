/*
 * statements: for statement
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

for (a =[3:5])echo(a);     // 3 4 5
for (a =[3:0]){echo(a);}   // 0 1 2 3         start > end is invalid, deprecated by 2015.3
for (a =[3:0.5:5])echo(a); // 3 3.5 4 4.5 5
for (a =[0:2:5])echo(a);   // 0 2 4           a never equals end
for (a =[3:-2:-1])echo(a); // 3 1 -1          negative increment requires 2015.3 be sure end < start

for (a =[3,4,1,5])echo(a); // 3 4 1 5
for (a =[0.3,PI,1,99]){echo(a);}    // 0.3 3.14159 1 99
x1=2; x2=8; x3=5.5;
for (a =[x1,x2,x3]){echo(a);} // 2 8 5.5
for (a =[[1,2],6,"s",[[3,4],[5,6]]])echo(a);  // [1,2] 6 "s" [[3,4],[5,6]]

animals = ["elephants", "snakes", "tigers", "giraffes"];
for(animal = animals)
    echo(str("I've been to the zoo and saw ", animal));
 // "I've been to the zoo and saw elephants", for each animal

for (i=[0:3])
    translate([i*10,0,0])
        cube(i+1);

// nested for
for(z=[-180:45:+180])
    for(x=[10:5:50])
        rotate([0,0,z]) translate([x,0,0]) cube(1);

// example for() nested 3 deep

color_vec = ["black","red","blue","green","pink","purple"];
for (x = [-20:10:20] )
    for (y = [0:4] )
        color(color_vec[y])
            for (z = [0,4,10] ) {
                translate([x,y*5-10,z])
                    cube();
            }

// shorthand nesting for same result

color_vec = ["black","red","blue","green","pink","purple"];
for (x = [-20:10:20],
    y = [0:4],
    z = [0,4,10] )
        translate([x,y*5-10,z]){color(color_vec[y])cube();}

// example 1 - iteration over a vector of vectors (rotation)
for(i = [ [  0,  0,   0],
    [ 10, 20, 300],
    [200, 40,  57],
    [ 20, 88,  57] ])
{
    rotate(i)
    cube([100, 20, 20], center = true);
}


// example 2 for() loop vector of vectors (translation)
for(i = [ [ 0,  0,  0],
        [10, 12, 10],
        [20, 24, 20],
        [30, 36, 30],
        [20, 48, 40],
        [10, 60, 50] ])
{
    translate(i)
    cube([50, 15, 10], center = true);
}

// example 3 for() loop vector of vectors
for(i = [ [[ 0,  0,  0], 20],
          [[10, 12, 10], 50],
          [[20, 24, 20], 70],
          [[30, 36, 30], 10],
          [[20, 48, 40], 30],
          [[10, 60, 50], 40] ])
{
  translate([i[0][0], 2*i[0][1], 0])
  cube([10, 15, i[1]]);
}

// example 1 - loop over a range:
intersection_for(n = [1 : 6])
{
    rotate([0, 0, n * 60])
    {
        translate([5,0,0])
        sphere(r=12);
    }
}

// example 2 - rotation :
 intersection_for(i = [ [  0,  0,   0],
 			[ 10, 20, 300],
 			[200, 40,  57],
 			[ 20, 88,  57] ])
{
    rotate(i)
    cube([100, 20, 20], center = true);
}
