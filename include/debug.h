#pragma once

/*
 * debugging utils
 *
 * Copyright © 2022 Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * ODOX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ODOX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ODOX.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <string>

// taken from [C++ Trick — Easy Debugging / Logging - Codeforces](https://codeforces.com/blog/entry/91347)
#ifndef NDEBUG

#define deb(...) logger(#__VA_ARGS__, __VA_ARGS__)

template<typename ...Args>
void logger(std::string vars, Args&&... values) {
  std::cout << vars << " = ";
  std::string delim = "";
  (..., (std::cout << delim << values, delim = ", "));
  std::cout << std::endl;
}
#else
#define deb(...)
#endif // NDEBUG
