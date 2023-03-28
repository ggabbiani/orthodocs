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
#include <commons/utils.h>

using namespace std;

namespace fs=std::filesystem;

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
