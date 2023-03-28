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

#include <spdlog/spdlog.h>

#include <cstring>

using namespace std;
namespace fs = std::filesystem;

const char * RcException::what() const noexcept {
  return _what;
}

const char * RcException::prolog() const noexcept {
  return "RcException";
}

void RcException::set(const char *s) noexcept {
  strncpy(_what,s,sizeof _what -1);
  _what[sizeof _what -1]  = 0;
}

FileNotFound::FileNotFound(const fs::path &fname) noexcept
  : RcException{exit_code} {
  set(fname.c_str());
}

const char * FileNotFound::prolog() const noexcept {
  return "FileNotFound";
}

OsError::OsError(const string &message) noexcept
  : RcException{exit_code} {
  set(message.c_str());
}

const char * OsError::prolog() const noexcept {
  return "OsError";
}

namespace exceptions {

void print(const exception& e, int level) {
  if (auto rce = dynamic_cast<const RcException*>(&e))
    spdlog::error("{0}{1}: {2}",string(level*2, ' '),rce->prolog(),rce->what());
  else
    spdlog::error("{0}{1}",string(level*2, ' '),e.what());
  try {
    rethrow_if_nested(e);
  } catch(const exception &nestedException) {
    print(nestedException, level+1);
  } catch(...) {}
}

} // namespace exceptions
