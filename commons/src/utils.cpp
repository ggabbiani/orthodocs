/*
 * general utilities definition
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <commons/error_info.h>
#include <commons/globals.h>
#include <commons/utils.h>

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>
#include <sstream>

using namespace std;

namespace fs=std::filesystem;

void print_exception(const exception& e, int level) {
  spdlog::error("{0}{1}",string(level, ' '),e.what());
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
