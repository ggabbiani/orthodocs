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

#include <filesystem>
#include <stdexcept>

/**
 * On unattended systems can be difficult to have the what() text shown,
 * the aim of this class is to provide also a custom return code to be
 * used by the exception handler.
 */
struct RcException : virtual std::exception {
  explicit RcException(int rc) : rc{rc} {}
  const int rc;
};

class FileNotFound : public std::runtime_error, virtual RcException {
public:
  explicit FileNotFound(const std::filesystem::path &fname);

  static constexpr int exit_code{-1};
};

class OsError : public std::runtime_error, virtual RcException {
public:
  explicit OsError(const std::string &message);

  static constexpr int exit_code{-2};
};
