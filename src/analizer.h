#pragma once
/*
 * source file analysis
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <commons/document.h>

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

  explicit Analizer(language::Extension *language): _language(language) {}

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

  /**
   * collect cross-reference data in the documents
   */
  void xref() const;

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
  language::Extension   *_language;
};
