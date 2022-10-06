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

#include "orthodocs/bar.h"
#include "orthodocs/config.h"
#include "orthodocs/extensions.h"
#include "orthodocs/globals.h"
#include "singleton.h"

using namespace std;

#ifdef OPTION_LANGUAGE_SCAD
extern "C" language::Extension *scad_extension;
#endif // OPTION_LANGUAGE_SCAD

#ifdef OPTION_WRITER_MARKDOWN
extern "C" writer::Extension *markdown_extension;
#endif // OPTION_WRITER_MARKDOWN

namespace language {

Extension *Extension::factory() {
  static language::Extension::Map registry = {
  #ifdef OPTION_LANGUAGE_SCAD
  {scad_extension->id,scad_extension}
  #endif // OPTION_LANGUAGE_SCAD
  };

  if (auto i=registry.find(Option::language()); i!=registry.end())
    return i->second;
  throw domain_error("No language extension found for id '"+Option::language()+'\'');
}

} // namespace language

namespace writer {

Extension *Extension::factory() {
  const writer::Extension::Map registry = {
  #ifdef OPTION_WRITER_MARKDOWN
  {markdown_extension->id,markdown_extension}
  #endif // OPTION_WRITER_MARKDOWN
  };

  if (auto i=registry.find(Option::writer()); i!=registry.end())
    return i->second;
  throw domain_error("No language extension found for id '"+Option::writer()+'\'');
}

void Extension::save(const orthodocs::DocumentList &docs) {
  try {
    orthodocs::Bar bar(docs,"documents saved");
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
