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
extern "C" language::Extension::Builder scad_builder;
#endif // OPTION_LANGUAGE_SCAD

#ifdef OPTION_WRITER_MARKDOWN
extern "C" writer::Extension::Builder markdown_builder;
#endif // OPTION_WRITER_MARKDOWN

namespace language {

Extension *Extension::factory(const std::string &language_id) {
  #ifdef OPTION_LANGUAGE_SCAD
  if (Extension *result=scad_builder(language_id); result)
    return result;
  #endif // OPTION_LANGUAGE_SCAD

  throw domain_error("No language extension found for '"+Option::language()+"' id");
}

} // namespace language

namespace writer {

Extension *Extension::factory(const string &writer_id,XRef &xref) {
  #ifdef OPTION_WRITER_MARKDOWN
  if (Extension *result=markdown_builder(writer_id,xref); result)
    return result;
  #endif // OPTION_WRITER_MARKDOWN

  throw domain_error("No writer extension found for '"+Option::writer()+"' id");
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
