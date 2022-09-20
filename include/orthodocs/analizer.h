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

#include "orthodocs/document.h"

#include "antlr4-runtime.h"

#include <filesystem>
#include <iostream>
#include <memory>

namespace language {

class Extension;

} // namespace language

namespace writer {

class Extension;

} // namespace writer

namespace orthodocs {

/**
 * Analize sources producing the Document through a language parser listener
 */
class Analizer {
public:
  using DocumentList = std::vector<std::unique_ptr<Document>>;

  explicit Analizer(language::Extension *language): _parser(language) {}

  // analize a set of files / directories
  void process(const FileSet &sources);
  
  /**
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

  // documents getter
  const DocumentList &documents() const {return _docs;}

  // ToC getter
  const doc::ToC &toc() const {return _toc;}

private:
  // return a FileSet made of files matching «extension»
  void lookup(
    // list of source directories/files
    const FileSet &sources,
    // extension to filter out
    const char *extension, 
    // resulting list of source files matching «extension»
    FileSet &result
  );
  DocumentList         _docs;
  doc::ToC             _toc;
  language::Extension *_parser;
};

}
