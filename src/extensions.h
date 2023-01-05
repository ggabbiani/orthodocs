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

#include "document.h"
#include "globals.h"
#include "singleton.h"

#include <filesystem>
#include <map>
#include <string>

namespace language {

class Extension {
public:
  using Analysis    = ::doc::xref::Analysis;
  using Annotation  = ::doc::Annotation;
  using Builder     = Extension *(*)(std::string_view language_id);
  using Dictionary  = ::doc::xref::Dictionary;
  using Vocabulary  = ::doc::xref::Vocabulary;

  explicit Extension(const char *id) : id(id) {}
  virtual ~Extension() = default;

  /**
   * Returns the SCAD extension instance if the passed language_id matches, null otherwise.
   */
  static Extension *factory(const std::string &language_id);

  virtual Document::Owner parse(const std::filesystem::path &source) const = 0;
  /**
   * Analizes a document annotation for cross-references and enrich the 
   * annotation with the analysis results.
   */
  virtual void analize(Annotation &anno) const = 0;

  /**
   * returns the Lexer Vocabulary
   */
  const Vocabulary &vocabulary() const {
    return _vocabulary;
  }

  /**
   * return the source postfix
   */
  virtual const char *sourcePostfix() const = 0;

  const char * const id;

protected:
  Vocabulary _vocabulary;
};

} // namespace language

namespace writer {

class Extension {
public:
  using Dictionary    = ::doc::xref::Dictionary;
  using Document      = ::Document;
  using DocumentList  = ::DocumentList;
  using Item          = ::doc::Item;
  using Language      = language::Extension;
  using ToC           = ::doc::ToC;
  using Vocabulary    = ::doc::xref::Vocabulary;

  using Builder       = Extension *(*)(std::string_view writer_id,Dictionary &dict,const Language *lang);

  Extension(const char *writer_id,Dictionary &dict,const Vocabulary &voc, const Language *lang) : id(writer_id),_dict(&dict),_language(lang),_vocabulary(voc) {}
  virtual ~Extension() = default;

  /**
   * Returns the Markdown extension instance if the passed writer_id matches, null otherwise.
   */
  static Extension *factory(const std::string &writer_id,Dictionary &dict,const Language *lang);
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

protected:
  Dictionary        *_dict;
  const Language    *_language;
  const Vocabulary  &_vocabulary; // Lexer literals

private:
};

} // namespace writer
