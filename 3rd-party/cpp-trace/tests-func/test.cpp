
#include "debug/trace.h"

#include <termcolor/termcolor.hpp>
#include <iostream>

using namespace std;

void nested_3() {
  TR_FUNC;
  TR_MSG("Message from inside nested function");
}

void nested_2() {
  TR_FUNC;
  TR_MSG("Message from inside nested function");
  nested_3();
}

void nested_1(const char *text) {
  double p1 = 2.71;
  int p2 = -321;
  const char *p3 = "\"I'm a C string\"";
  string p4 = "\"I'm a C++ string\"";
  TR_FUNC;
  TR_MSG(text);
  nested_2();
  TR_MSG("This is a message with a lot of parameters", p1, p2, p3, p4);
}

int main(int argc, const char *argv[]) {
  double pi = 3.14;
  TR_FUNC;
  TR_MSG("ciao");
  TR_MSG("hello");
  {
    TR_NEW("nested block");
    TR_MSG("Message from inside a nested block of instructions");
  }
  TR_MSG("This is π:", pi);
  nested_1("test string");
}