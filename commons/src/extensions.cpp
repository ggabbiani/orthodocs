/*
 * extension management implementation
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

#include "bar.h"
#include "extensions.h"
// #include "globals.h"
// #include "singleton.h"

using namespace std;

namespace writer {

void Extension::save(const ::DocumentList &docs) {
  try {
    ::Bar bar(docs,"documents saved");
    for(const auto &doc: docs) {
      bar.status(doc->source.string());
      save(*doc);
      bar++;
    }
  } catch(...) {
    indicators::show_console_cursor(true);
    throw;
  }
}

} // namespace writer
