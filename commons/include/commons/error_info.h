#pragma once
/*
 * insert a brief description here
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <sstream>
#include <string>

// see [C++ Trick — Easy Debugging / Logging](https://codeforces.com/blog/entry/91347)
#define ERR_CALL(...) std::string(__FILE__)+":"+std::to_string(__LINE__)+" - "+std::string(__func__)+"("+::details::arguments(__VA_ARGS__)+")"

#define ERR_INFO std::string(__FILE__)+":"+std::to_string(__LINE__)+" - "

namespace details {

template<typename ...Args>
std::string arguments(Args&&... values) {
  std::ostringstream out;
  std::string delim = "";
  // [fold expression(since C++17)](https://en.cppreference.com/w/cpp/language/fold)
  (..., (out << delim << values, delim = ", "));
  return out.str();
}

}