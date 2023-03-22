/*
 * Common used exceptions.
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <commons/exceptions.h>

using namespace std;
namespace fs = std::filesystem;

FileNotFound::FileNotFound(const fs::path &fname)
  : RcException{exit_code}
  , std::runtime_error(fname.string()+std::string(" not found")) {
}

OsError::OsError(const string &message)
  : RcException{exit_code}
  , std::runtime_error("OS Error: " + message) {
}
