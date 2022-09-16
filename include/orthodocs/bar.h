#pragma once
/*
 * Empty file description
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
 * along with ODOX.  If not, see <http: //www.gnu.org/licenses/>.
 */

#include "indicators/block_progress_bar.hpp"
#include "indicators/cursor_control.hpp"

namespace orthodocs {

template <class T>
class Bar {
public:
  Bar(const T &container,const char *type,size_t width=50) {
    indicators::show_console_cursor(false);
    _bar.set_option(indicators::option::BarWidth{width});
    _bar.set_option(indicators::option::MaxProgress{container.size()});
    _bar.set_option(indicators::option::ForegroundColor{indicators::Color::green});
    _end = "✔ "+std::to_string(container.size())+" "+type;
  }
  // set text as postfix
  void status(const std::string &text) {
    _bar.set_option(indicators::option::PostfixText{text});
  }
  // update progress bar
  void operator ++(int) {
    _bar.tick();
  }
  ~Bar() {
    status(_end);
    _bar.mark_as_completed();
    indicators::show_console_cursor(true);
  }
private:
  indicators::BlockProgressBar _bar;
  std::string _end;
};

} // namespace orthodocs
