#pragma once
/*
 * Simple singleton implementation
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

template <typename T>
class Singleton {
public:
  template <typename ... Args>
  static T& instance(Args&& ... args) {
    static T i(std::forward<Args>(args) ...);
    return i;
  }
private:
  Singleton(const Singleton&)        = delete;
  void operator = (const Singleton&) = delete;
};
