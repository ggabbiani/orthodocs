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

#include "debug/trace.h"

#include <termcolor/termcolor.hpp>

using namespace std;

namespace debug {

std::string repeat(std::string str, const size_t n) {
  if (n == 0) {
    str.clear();
    str.shrink_to_fit();
    return str;
  } else if (n == 1 || str.empty()) {
    return str;
  }
  const auto period = str.size();
  if (period == 1) {
    str.append(n - 1, str.front());
    return str;
  }
  str.reserve(period * n);
  std::size_t m {2};
  for (; m < n; m *= 2) str += str;
  str.append(str.c_str(), (n - (m / 2)) * period);
  return str;
}

Trace::Trace(const std::string& name) : _name(name) {
  std::cout  << _prefix << open() << _name << std::endl;
  right();
}

Trace::~Trace() {
  left();
  std::cout << _prefix << close() << _name << std::endl;
}

std::string Trace::open() const {
  _stack.emplace(_color[(_n+1) % 3]);
  return _stack.top()+"┌ "+_reset+'+';
}

void Trace::right() const {
  _n++;
  _level  = _stack.top()+_bar+_reset;
  _prefix += _level;
}

void Trace::left() const {
  _n--;
  _prefix = _prefix.substr(0,_prefix.length()-_level.length());
  _stack.pop();
}

std::string Trace::close() const {
  return string(_color[(_n+1) % 3])+"└ "+_reset+'-';
}

int           Trace::_n  = 0;
string        Trace::_prefix;
string        Trace::_level;
stack<string> Trace::_stack;

const string Trace::_bar = {"│ "};
const string Trace::_reset = {"\033[00m"};
const string Trace::_color[] = {
  {"\033[33m"}, // yellow
  {"\033[35m"}, // magenta
  {"\033[34m"}, // blue
};

}
