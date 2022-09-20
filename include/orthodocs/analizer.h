#pragma once
/*
 * source file analysis
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

#include "orthodocs/extensions.h"
#include "orthodocs/document.h"

#include "antlr4-runtime.h"

#include <filesystem>
#include <iostream>
#include <memory>

namespace orthodocs {

/**
 * Analize sources producing the Document via a scad parser listener
 */
class Analizer {
public:
  explicit Analizer() : _writer(writer::Extension::factory()),_parser(language::Extension::factory()) {
  }

  void process(const FileSet &sources);
  
  void writeToC();

  void writeGraphs(const FileSet &dirs);

  /**
   *
   * 
   * 1) reset the currently held document (deleting the eventually already present one)
   * 2) Produce the synthetic document of the source code passed as argument
   * 3) invoke the writer to write it on the doc root
   * 4) move the document items in the ToC
   * 
   * At the end of the process the document is empty.
   */
  void document(
    // source file relative to source root
    const std::filesystem::path &source
  );
  std::unique_ptr<Document> doc;
private:
  doc::ToC             _toc;
  writer::Extension   *_writer;
  language::Extension *_parser;
};

}
