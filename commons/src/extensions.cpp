/*
 * extension management implementation
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <commons/bar.h>
#include <commons/extensions.h>

using namespace std;

namespace writer {

void Extension::save(const ::Document::List &docs) {
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
