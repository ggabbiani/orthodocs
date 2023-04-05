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
  return _what.data();
}

const char * RcException::prolog() const noexcept {
  return "RcException";
}

/*
  * path::value_type represents the character type used by the native encoding
  * of the filesystem:
  * - char on POSIX/UTF-8
  * - wchar_t on Windows/UTF-16
  * See also [std::filesystem::path::string - cppreference.com](https://en.cppreference.com/w/cpp/filesystem/path/string)
  * and [std::filesystem::path::generic_string](https://en.cppreference.com/w/cpp/filesystem/path/generic_string)
  */
void RcException::set(const fs::path &p) noexcept {
  // native format path is automatically converted
  set(p.generic_string());
}

void RcException::set(const string &s) noexcept {
  const auto n = _what.max_size();
  _what[n-1]  = 0;
  strncpy(_what.data(),s.c_str(),n);
  if (_what[n-1]!=0) {
    // this works assuming sizeof _what >= 4
    assert(n>=4);
    // truncation occurred, just signal it appending an ellipsis and secure
    // the solution zeroing the destination buffer
    _what[n-4]  = '.';
    _what[n-3]  = '.';
    _what[n-2]  = '.';
    _what[n-1]  = 0;
  }
}

FileNotFound::FileNotFound(const fs::path &fname) noexcept
  : RcException{exit_code} {
  set(fname);
}

const char * FileNotFound::prolog() const noexcept {
  return "FileNotFound";
}

OsError::OsError(const string &message) noexcept
  : RcException{exit_code} {
  set(message);
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
