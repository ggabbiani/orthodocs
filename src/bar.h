#pragma once
/*
 * Progress indicator bar.
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
