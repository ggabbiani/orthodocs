if (func_1()==4)
  mod_3();
else
  mod_4();

if (v==1)
  mod_0();
if (v2) {
  mod_1();
  mod_2();
}

if (v==1) {
  mod_0();
  mod_1();
} if (v2)
  mod_2();

if (func_2(p)) {
  mod_1();
  mod_2();
} else {
  mod_1();
  mod_2();
}

if (b==a)  cube(4);
if (b<a)  {cube(4); cylinder(6);}
if (b&&a) {cube(4); cylinder(6);}
if (b!=a)  cube(4); else cylinder(3);
if (b)    {cube(4); cylinder(6);} else {cylinder(10,5,5);}
if (!true){cube(4); cylinder(6);} else  cylinder(10,5,5);
if (x>y)   cube(1, center=false); else {cube(size = 2, center = true);}
if (a==4) {}                      else  echo("a is not 4");
if ((b<5)&&(a>8))  {cube(4);}     else {cylinder(3);}
if (b<5&&a>8)       cube(4);      else  cylinder(3);

/*
 * Nested if: The scopes of both the if() portion and the else portion, can in
 * turn contain if() statements. This nesting can be to many depths.
 */
if (test1) {
  scope1()
    if (test2) {scope2_1()}
    else {scope2_2()}
} else {
  scope2()
    if (test3) {scope3_1()}
    else {scope3_2()}
}

if((k<8)&&(m>1)) cube(10);
else if(y==6)   {sphere(6);cube(10);}
else if(y==7)    color("blue")sphere(5);
else if(k+m!=8) {cylinder(15,5,0);sphere(8);}
else             color("green"){cylinder(12,5,0);sphere(8);}

