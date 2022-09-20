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
 * along with ODOX.  If not, see <http: //www.gnu.org/licenses/>.
 */

#include "orthodocs/analizer.h"
#include "orthodocs/bar.h"
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
    doc.reset(_parser->parse(source));
    // document writing
    _writer->document(source,doc.get());
    // merge document contents to the Table of Contents
    doc::toc::add(doc.get(),_toc);
  } catch(...) {
    throw_with_nested(runtime_error("error while processing '"+source.string()+'\''));
  }
}

void Analizer::process(const FileSet &sources) {
  try {
    Bar bar(sources,"Documents");
    for(const auto &file: sources) {
      bar.status(file.string());
      document(file);
      bar++;
    }
  } catch(...) {
    indicators::show_console_cursor(true);
    throw;
  }
}

void Analizer::writeGraphs(const FileSet &dirs) {
  _writer->graphs(_toc,dirs);
}

void Analizer::writeToC() {
  _writer->toc(_toc);
}

}
