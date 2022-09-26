/*
 * source file processor.
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

#include "error_info.h"
#include "orthodocs/analizer.h"
#include "orthodocs/bar.h"
#include "orthodocs/extensions.h"
#include "orthodocs/globals.h"
#include "orthodocs/utils.h"

using namespace std;
using namespace antlr4;

namespace fs = std::filesystem;

namespace orthodocs {

void Analizer::document(const fs::path &source) {
  assert(source.is_relative());
  assert(source.has_filename());
  try {
    auto document = _parser->parse(source);
    // copy document contents to the Table of Contents
    doc::toc::add(document.get(),_toc);
    // move document in the documents list
    _docs.emplace(_docs.end(),std::move(document));
  } catch(...) {
    throw_with_nested(runtime_error(ERR_CALL(source)));
  }
}

void Analizer::process(const FileSet &sources) {
  try {
    FileSet files;
    lookup(sources,_parser->sourcePostfix(),files);

    Bar bar(files,"analized sources");
    for(const auto &file: files) {
      bar.status(file.string());
      document(file);
      bar++;
    }
  } catch(...) {
    indicators::show_console_cursor(true);
    throw;
  }
}

void Analizer::lookup(const FileSet &sources, const char *extension, FileSet &result) {
  cwd pwd(Option::sroot());
  for(auto &path: sources) {
    if (fs::is_regular_file(path)) {
      if (!extension || path.extension()==extension)
        result.insert(path);
    } else if (fs::is_directory(path)) {
      for (auto &entry: fs::directory_iterator{path}) {
        const auto &entry_path = entry.path();
        if (fs::is_regular_file(entry_path)) {
          if (!extension || entry_path.extension()==extension)
            result.insert(entry_path);
        } else if (fs::is_directory(entry_path)) {
          lookup(FileSet{entry_path},extension,result);
        }
      }
    } else 
      throw runtime_error("what is this '"+path.string()+"'?");
  }
}

}
