#pragma once
/*
 * Progress indicator bar.
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later */

#include "globals.h"

#include <spdlog/spdlog.h>

#include <indicators/block_progress_bar.hpp>
#include <indicators/cursor_control.hpp>

template <class T>
class Bar {
public:
  Bar(const T &container,const char *type,size_t width=50) : _end{"✔ "+std::to_string(std::size(container))+" "+type} {
    indicators::show_console_cursor(false);
    _bar.set_option(indicators::option::BarWidth{width});
    _bar.set_option(indicators::option::MaxProgress{container.size()});
    _bar.set_option(indicators::option::ForegroundColor{indicators::Color::green});
  }
  Bar(const Bar&) = delete;
  Bar(const Bar&&) = delete;
  Bar & operator = (const Bar &) = delete;
  Bar & operator = (Bar &&) = delete;
  // set text as postfix
  void status([[maybe_unused]] const std::string_view &text) {
    if (Option::verbosity()<=Option::Verbosity::info || Option::verbosity()!=Option::Verbosity::off) {
      _bar.set_option(indicators::option::PostfixText{text});
      _bar.print_progress();
    }
  }
  // update progress bar
  void operator ++(int) {
    if (Option::verbosity()<=Option::Verbosity::info || Option::verbosity()!=Option::Verbosity::off)
      _bar.tick();
  }
  ~Bar() {
    try {
      if ((Option::verbosity()<=Option::Verbosity::info || Option::verbosity()!=Option::Verbosity::off)
        && !std::uncaught_exceptions()) {
          status(_end);
          _bar.mark_as_completed();
      }
    } catch(...) {
      indicators::show_console_cursor(true);
    }
    indicators::show_console_cursor(true);
  }

private:
  indicators::BlockProgressBar        _bar;
  std::string                         _end;
};
