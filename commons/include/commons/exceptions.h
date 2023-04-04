#pragma once
/*
 * Common used exceptions.
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <array>
#include <filesystem>
#include <stdexcept>

namespace exceptions {

/*!
 * prints the explanatory string of an exception. If the exception is nested,
 * recurses to print the explanatory of the exception it holds
 */
extern void print(const std::exception& e, int level =  0);

} // namespace exception

/**
 * On unattended systems can be difficult to have the what() text shown,
 * the aim of this class is to provide also a custom return code to be
 * used by the exception handler.
 */
class RcException : public virtual std::exception {
public:
  friend void exceptions::print(const std::exception& e, int level);
  explicit RcException(int rc) noexcept : rc{rc} {}
  const char * what() const noexcept override;

  const int rc;

protected:
  void set(const std::filesystem::path &p) noexcept;
  void set(const std::string &s) noexcept;
  virtual const char * prolog() const noexcept;

private:
  std::array<char,240>  _what;
};

class FileNotFound : public virtual RcException {
public:
  explicit FileNotFound(const std::filesystem::path &fname) noexcept;
  const char * prolog() const noexcept override;

  static constexpr int exit_code{-1};
};

class OsError : public virtual RcException {
  using RcException::what;
public:
  explicit OsError(const std::string &message) noexcept;
  const char * prolog() const noexcept override;

  static constexpr int exit_code{-2};
};
