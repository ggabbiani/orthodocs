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
// #include <debug/trace.h>

#include <filesystem>
#include <sstream>
#include <string>

// see [C++ Trick — Easy Debugging / Logging](https://codeforces.com/blog/entry/91347)
#define ERR_CALL(...) std::string(__FILE__)+":"+std::to_string(__LINE__)+" - "+std::string(__func__)+"("+::details::arguments(__VA_ARGS__)+")"

#define ERR_INFO std::string(__FILE__)+":"+std::to_string(__LINE__)+" - "

namespace details {

template <typename T>
std::string parametrize(T &&p) {
  // TR_FUNC;
  return std::forward<T>(p);
}

template <typename T>
std::string parametrize(T *p) {
  // TR_FUNC;
  std::ostringstream out;
  out << p;
  return out.str();
}

inline std::string parametrize(std::string &&s) {
  // TR_FUNC;
  return '"'+s+'"';
}

inline std::string parametrize(const std::filesystem::path &p) {
  // TR_FUNC;
  std::filesystem::path preferred{p};
  return parametrize(preferred.make_preferred().generic_string());
}

/*
 * this should not exists, but is a bizarre workaround to fix problems with gcc
 * <v12.2.1 on fold expressions.
 */
inline std::string parametrize(std::filesystem::path &p) {
//  TR_FUNC;
  std::filesystem::path preferred{p};
  return parametrize(preferred.make_preferred().generic_string());
}

template<typename ...Args>
std::string arguments(Args&&... values) {
  // TR_FUNC;
  std::ostringstream out;
  std::string delim = "";
  // [fold expression(since C++17)](https://en.cppreference.com/w/cpp/language/fold)
  (..., (out << delim << parametrize(values), delim = ", "));
  return out.str();
}

}