ident_1     = true;
ident_2     = false;
ident_3     = undef;
// expr == NUMBER
ident_4     = 1;
ident_5     = +1;
ident_6     = -1;
ident_7     = 1.3;
ident_8     = +1.3;
ident_9     = -1.3;
ident_10    = 1.3e3;
ident_11    = +1.3e3;
ident_12    = -1.3e3;
// strings
i = "I am a white smiling face: ☺";
// lookup
a = b;
// range expressions
r1 = [0:10];
r2 = [0.5:2.5:20];
// list expression
a1  = [1,2,3];
a2  = [4,5];
a3  = [6,7,8,9];
b   = [a1,a2,a3];
e   = [ [1], [], [3,4,5], "string", "x", [[10,11],[12,13,14],[[15,16],[17]]] ];
// expr [+-*/%] expr
i = a + b;
a = a - b + c * d / e % f;
// booleans
condition = (a+2) >= 3;
condition   = !false;
// [+-]expr
value       = +3.14;
value       = a - 3.14 * (20 / c);
// ternary operator
identifier = (a+2) > 0 ? "ciao" : "fuckoff";
// lists
list    = [1,2,3];
list    = [a,5,b];
list    = [];
list    = [5.643];
list    = ["a","b","string"];
list    = [[1,r],[x,y,z,4,5]];
list    = [3, 5, [6,7], [[8,9],[10,[11,12],13], c, "string"]];
// list    = [4/3, 6*1.5, cos(60)];
//
a[b*5]  = 0;
a       = b[2-c];
a[b*5]  = b[2-c];
a[b[c]-2*d] = e[f[g]-2*h];
// let statement
identifier = let (angle = i*360/20, r= i*2, distance = r*5) angle + 2;
// function call
identifier = func(b[2],"this is a string");
