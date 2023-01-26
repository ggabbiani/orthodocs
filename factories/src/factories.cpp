/*
 * extension factories implementation
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <commons/config.h>
#include <commons/extensions.h>

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
