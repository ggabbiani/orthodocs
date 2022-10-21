/*
 * general utilities definition
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

#include "orthodocs/error_info.h"
#include "orthodocs/globals.h"
#include "orthodocs/utils.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>
#include <sstream>

using namespace std;

namespace fs=std::filesystem;

void print_exception(const exception& e, int level) {
  cerr << string(level, ' ') << "***EXCEPTION***: " << e.what() << '\n';
  try {
    rethrow_if_nested(e);
  } catch(const exception &nestedException) {
    print_exception(nestedException, level+1);
  } catch(...) {}
}

bool is_sub_of(const fs::path &sub, const fs::path &base) {
  for (auto b = base.begin(), s = sub.begin(); b != base.end(); ++b, ++s) {
    if (s==sub.end() || *s!=*b) {
      return false;
    }
  }
  return true;
}

cwd::cwd(const fs::path &path) {
  try {
    fs::current_path(path.empty() ? fs::current_path() : path);
  } catch(...) {
    throw_with_nested(runtime_error(ERR_CALL(path)));
  }
}
