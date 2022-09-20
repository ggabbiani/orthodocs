#pragma once

#include <sstream>
#include <string>

// see [C++ Trick — Easy Debugging / Logging](https://codeforces.com/blog/entry/91347)
#define ERR_CALL(...) std::string(__FILE__)+":"+std::to_string(__LINE__)+" - "+std::string(__func__)+"("+arguments(__VA_ARGS__)+")"
template<typename ...Args>
std::string arguments(Args&&... values) {
  std::ostringstream out;
  std::string delim = "";
  (..., (out << delim << values, delim = ", "));
  return out.str();
}
