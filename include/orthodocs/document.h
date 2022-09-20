#pragma once
/*
 * abstract document
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

#include "orthodocs/utils.h"

#include <filesystem>
#include <functional>
#include <map>
#include <stack>
#include <string>
#include <vector>

namespace orthodocs {

namespace doc {

using Annotation  = std::string;
//! Item or parameter identifier
using Name        = std::string;
//! used for both function and modules
using Signature   = std::string;
//! always relative to document root
using URI         = std::filesystem::path;
//! defaults for variables and parameters
using Value       = std::string;

class Parameter {
public:
  const Name &name() const {return _name;}
  const Annotation &annotation() const {return _annotation;}
  Value       defaults;
// protected:
  Name        _name;
  Annotation  _annotation;
};
using ParameterPtr  = std::unique_ptr<Parameter>;
using ParameterVec  = std::vector<ParameterPtr>;

class Item {
  friend class Index;
public:
  virtual ~Item() = default;

  virtual std::string type() const = 0;

  /**
   * builds key value usable by Document
   * 
   * «item type» «item name»
   */
  virtual std::string documentKey() const;

  /**
   * builds a key value usable by Index.
   * 
   * «item name» («item type»)
   */
  virtual std::string indexKey() const;

  Name          name;
  Annotation    annotation;
  ParameterVec  parameters;
  const Value   defaults;
  const bool    nested;
  /**
   * the semantic of this field is language dependant
   */
  Item          *parent = nullptr;
  // filled by writers
  URI           uri;

protected:
  Item(const Name &name,const Value *defaults=nullptr,bool nested=false) : name(name),nested(nested),defaults(defaults?*defaults:"") {}
  Signature signature() const; 
};

using ItemPtr       = std::unique_ptr<Item>;
using ConstItemPtr  = std::unique_ptr<const Item>;
// used by generators for stackable items (modules and variables)
using ItemPtrStack  = std::stack<ItemPtr>;

/**
 * Table of Contents
 * NOTE: no ownership of the contained items
 * FIXME: a std::map couldn't be enough for this?
 */
using ToC = std::multimap<Name,Item*,nocase::Compare>;

/**
 * SubToC is a filtered selection of ToC elements, as such its items are 
 * simple pointers, because a SubToC has no ownerships over them.
 * It is critical that SubToC never survive its ToC.
 */
using SubToC = std::multimap<Name,doc::Item*,nocase::Compare>;

} // namespace doc

class Document {
public:
  explicit Document(const std::filesystem::path &source) : source(source) {}
  /**
   * Document::Index format:
   * Key    ==> "function|module|variable <item name>"
   * Value  ==> unique_ptr<doc::Item>
   * 
   * valid key examples:
   * 
   * "variable $FL_ADD"
   * "function fl_description"
   * "module fl_manage"
   * "package defs"
   * 
   * NOTE: see doc::Key()
   */
  using Index = std::map<std::string,doc::ItemPtr>;
  /**
   * return the number of item of type «type»
   */
  template <class T>
  size_t size() const {
    size_t size  = 0;
    for(auto i=index.begin(); i!=index.end(); ++i) {
      auto item = i->second.get();
      size += (typeid(*item)==typeid(T));
    }
    return size;
  }

  Index::iterator begin() {return index.begin();}
  Index::const_iterator begin() const {return index.begin();}
  Index::iterator end() {return index.end();}
  Index::const_iterator end() const {return index.end();}

// private:
  Index index;
  std::filesystem::path source;
};

namespace doc {

namespace toc {

/**
 * build an index title in the form
 * 
 * "«item name» («item type»)"
 */
inline std::string title(const Item &item) {
  return item.name+" ("+item.type()+')';
}

// copy Document items into the Table of Contents
inline void add(Document *document, ToC &toc) {
  for(auto &item: *document) 
    toc.emplace(item.second->indexKey(),item.second.get());
}

/**
 * returns a «func()» filtered SubToC
 */
extern SubToC filter(const std::filesystem::path &path,const ToC &toc, std::function<bool(const std::filesystem::path&,const Item*)> func);

} // namespace toc

} // namespace doc


} // namespace orthodocs
