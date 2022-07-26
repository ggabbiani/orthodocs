module mod_1(p1,p2) {
  var = func_1(1,2);
  children();
  pippo() {}
  pluto() {
    first(2,"ciao");
    second(0);
  }
}

module mod_2(p1, p2, p3=3) {
  if (p1)
    mod_3(p2,p3) children();
  else
    mod_4(p2,p3) children();
}
