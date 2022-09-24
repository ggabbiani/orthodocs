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

#include "orthodocs/analizer.h"
#include "orthodocs/document.h"
#include "orthodocs/globals.h"
#include "singleton.h"

#include <filesystem>
#include <map>
#include <string>

template <class EXTENSION>
class ExtensionLoader {
public:
  ExtensionLoader() {
    EXTENSION &extension=Singleton<EXTENSION>::instance();
    auto &registry = Singleton<typename EXTENSION::Map>::instance();
    registry.emplace(extension.id,&extension);
  }
};

namespace language {

class Extension {
public:
  explicit Extension(const char *id) : id(id) {}
  using Map = std::map<std::string,Extension*>;
  const char * const id;
  virtual ~Extension() = default;
  virtual orthodocs::Document *parse(const std::filesystem::path &source) const = 0;
  /**
   * return the source postfix
   */
  virtual const char *sourcePostfix() const = 0;
  /**
   * return the extension corresponding to the Option::writer()
   */
  static Extension *factory();
};

} // namespace language

namespace writer {

class Extension {
public:
  explicit Extension(const char *id) : id(id) {}
  using Map = std::map<std::string,Extension*>;
  const char * const id;
  virtual ~Extension() = default;
  /**
   * return the extension corresponding to the Option::writer()
   */
  static Extension *factory();
  /**
   * write a set of documents
   */
  void save(const orthodocs::Analizer::DocumentList &docs);
  /**
   * write a document in the concrete instance native format
   */
  virtual void save(const orthodocs::Document &document) = 0;
  /**
   * write a table of contents in the concrete instance native format
   */
  virtual void save(const orthodocs::doc::ToC &toc) = 0;
  /**
   * write sub-root graphs
   */
  virtual void graphs(const orthodocs::doc::ToC &toc, const FileSet &dirs) = 0;
};

} // namespace writer
