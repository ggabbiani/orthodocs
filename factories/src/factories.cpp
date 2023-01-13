/*
 * extension factories implementation
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

#include "config.h"
#include "extensions.h"
// #include "globals.h"

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

  throw domain_error("No language extension found for '"+language_id+"' id");
}

} // namespace language

namespace writer {

Extension *Extension::factory(const string &writer_id,Dictionary &dict, const language::Extension *lang) {
  #ifdef OPTION_WRITER_MARKDOWN
  if (Extension *result=markdown_builder(writer_id,dict,lang); result)
    return result;
  #endif // OPTION_WRITER_MARKDOWN

  throw domain_error("No writer extension found for '"+writer_id+"' id");
}

} // namespace writer
