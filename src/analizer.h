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
 * along with ODOX.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "document.h"

#include <filesystem>
#include <iostream>
#include <memory>

namespace language {

class Extension;

} // namespace language

namespace writer {

class Extension;

} // namespace writer

/**
 * Analize sources producing the Document through a language parser listener
 */
class Analizer {
public:
  using Dictionary  = doc::xref::Dictionary;

  explicit Analizer(language::Extension *language): _parser(language) {}

  /**
   * analize a set of files / directories as specified from Option(s)
   */
  auto buildDocuments() ->void;
  /**
   * @brief populate a dictionary with candidate items
   * 
   * @return Dictionary 
   */
  auto populate() const -> Dictionary;

  /**
   * 1) produce an in-memory document of the source code passed
   * 2) add document contents to the Table of Contents
   * 3) populate item Dictionary
   * 4) move the in memory document in the document list
   */
  void document(
    // source file relative to source root
    std::filesystem::path source
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
  DocumentList           _docs;
  doc::ToC               _toc;
  language::Extension   *_parser;
};
