#pragma once

/*
 * C++ tracing facility library.
 *
 * Copyright © 2001-2022 Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * This file is part of the 'Cpp Trace library' (CPPTRACE) project.
 *
 * CPPTRACE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CPPTRACE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CPPTRACE.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <termcolor/termcolor.hpp>

#include <sstream>
#include <stack>

namespace debug {

extern std::string repeat(std::string str, const size_t n);

inline std::string operator * (std::string str, size_t n) {
  return repeat(std::move(str), n);
}

/**
 * Simple trace facility class based on RAII.
 */
class Trace {
  template <class T>
  friend Trace& operator << (Trace&, const T&);

public:
  explicit Trace(const std::string& name);
  explicit Trace(const char* name) : Trace(std::string(name)) {}
  ~Trace();
  /** trace applicator */
  Trace& operator << (Trace& (*f)(Trace&)) {return f(*this);}

private:
  void right() const;
  void left() const;

  std::string open() const;
  std::string close() const;

  const std::string               _name;

  static std::string              _prefix;
  static std::string              _level;
  static std::stack<std::string>  _stack;
  static int                      _n;

  static const std::string _color[];
  static const std::string _reset;
  static const std::string _bar;
};

/**
 * enqueue a value on the current trace message.
 */
template <class T>
Trace& operator << (Trace& tracer, const T& value) {
  std::cout << Trace::_prefix << "  " << value << std::endl;
  return tracer;
}

namespace details {

template<typename ...Args>
std::string arguments(Args&&... values) {
  std::ostringstream out;
  std::string delim = "";
  // [fold expression(since C++17)](https://en.cppreference.com/w/cpp/language/fold)
  (..., (out << delim << values, delim = " "));
  return out.str();
}

} // namespace details

}  // namespace debug

#ifndef NDEBUG
/** new trace object function named */
#define TR_FUNC debug::Trace __trace__(__PRETTY_FUNCTION__)
/** a new trace named by parameter
 * @param name the name of the trace object
 */
#define TR_NEW(name) debug::Trace __trace__(name)
/** enqueue a message to the current trace object
 * @param p the message constituited by one or more parameters concatenated through the << operator
 */
#define TR_MSG(...) __trace__ << debug::details::arguments(__VA_ARGS__)
/** trace object signaling a NOT IMPLEMENTED function */
#else
#define TR_FUNC
#define TR_NEW(name)
#define TR_MSG(...)
#endif
