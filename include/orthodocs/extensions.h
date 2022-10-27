#pragma once
/*
 * writer and language extension management.
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

#include "orthodocs/document.h"
#include "orthodocs/globals.h"
#include "orthodocs/xref.h"
#include "singleton.h"

#include <filesystem>
#include <map>
#include <string>

namespace language {

class Extension {
public:
  using Map   = std::map<std::string,Extension*,std::less<>>;
  using XRef  = orthodocs::doc::XRef;

  explicit Extension(const char *id) : id(id) {}
  virtual ~Extension() = default;

  virtual std::unique_ptr<orthodocs::Document> parse(const std::filesystem::path &source) const = 0;
  /**
   * return the source postfix
   */
  virtual const char *sourcePostfix() const = 0;
  /*
   * return the Annotation analyzer
   */
  virtual XRef::Analyzer analist() = 0;
  /**
   * return the extension corresponding to the Option::language()
   */
  static Extension *factory();

  const char * const id;

};

} // namespace language

namespace writer {

class Extension {
public:
  using Document      = orthodocs::Document;
  using DocumentList  = orthodocs::DocumentList;
  using Item          = orthodocs::doc::Item;
  using Map           = std::map<std::string,Extension*,std::less<>>;
  using ToC           = orthodocs::doc::ToC;
  using XRef          = orthodocs::doc::XRef;

  explicit Extension(const char *id) : id(id) {}

  /**
   * return the extension corresponding to the Option::writer()
   */
  static Extension *factory();
  /**
   * write a set of documents using the passed dictionary for annotation cross-references
   */
  void save(const DocumentList &docs);
  /**
   * write a document in the concrete instance native format
   */
  virtual void save(const Document &doc) = 0;
  /**
   * write a table of contents in the concrete instance native format
   */
  virtual void save(const ToC &toc) = 0;
  /**
   * write sub-root graphs
   */
  virtual void graphs(const ToC &toc, const FileSet &dirs) = 0;

  /**
   * return the concrete reference string to the passed doc::Item
   */
  virtual std::string reference(const Item *item, const std::filesystem::path *document_source=nullptr) const = 0;

  const char * const id;

  void set(XRef &xref) {
    _xref = &xref;
  }

protected:
  XRef &xref() const {
    return *_xref;
  }

private:
  XRef *_xref;
};

} // namespace writer
