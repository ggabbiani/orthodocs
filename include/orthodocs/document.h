#pragma once
/*
 * synthetic document
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

#include "orthodocs/error_info.h"
#include "orthodocs/utils.h"

#include <filesystem>
#include <functional>
#include <map>
#include <set>
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
//! default value for Variable and Parameter
using Value       = std::string;

class Parameter {
public:
  using Ptr  = std::unique_ptr<Parameter>;
  using Vec  = std::vector<Ptr>;

  Value       defaults;
  Name        name;
  Annotation  annotation;
};

class Item {
  friend class Index;
public:

  using Ptr = std::unique_ptr<Item>;
  // used by generators for stackable items (modules and variables)
  using PtrStack  = std::stack<Ptr>;

  virtual ~Item() = default;

  virtual std::string type() const = 0;

  /**
   * return wether the item identifier is private or not.
   * NOTE: it uses the global option private_prefix.
   */
  virtual bool privateId() const;
  /**
   * builds a key usable by Document::Index in the following format:
   * 
   * «item type» «item name»
   * 
   * examples:
   * 
   * "package artifacts/spacer"
   * "module fl_spacer"
   * "function fl_bb_spacer"
   * "variable FL_SPC_NS"
   */
  virtual std::string documentKey() const;

  /**
   * builds a key value usable by Index.
   *
   * «item name» («item type»)
   */
  virtual std::string indexKey() const;

  Name            name;
  Annotation      annotation;
  Parameter::Vec  parameters;
  const Value     defaults;
  const bool      nested;
  /**
   * the semantic of this field is language dependant
   */
  Item          *parent = nullptr;
  // filled by writers represent the uri of the saved document
  URI           uri;

protected:
  Item(const Name &name,const Value *defaults=nullptr,bool nested=false) : name(name),defaults(defaults?*defaults:""),nested(nested) {}
  Signature _signature() const;
};

/**
 * Table of Contents
 * NOTE: NO OWNERSHIP OF THE CONTAINED ITEMS
 */
using ToC = std::multimap<Name,Item*,nocase::Less>;

/**
 * SubToC is a filtered selection of ToC elements, as such its items are
 * simple pointers, because a SubToC has no ownerships over them.
 * It is critical that SubToC never survive its ToC.
 */
using SubToC = std::multimap<Name,doc::Item*,nocase::Less>;

} // namespace doc

class Document {
  using path      = std::filesystem::path;
public:
  using Ownership = std::unique_ptr<Document>;

  explicit Document(const path &source) : source(source) {}
  /**
   * CASE SENSITIVE Document::Index comparison functor.
   * NOTE: see doc::Item::documentKey() for the format
   */
  struct IndexLesser {
    inline bool operator() (const doc::Item::Ptr &lhs, const doc::Item::Ptr &rhs) const {
      return  lhs->documentKey() < rhs->documentKey();
    }
  };

  using Index = std::set< doc::Item::Ptr, IndexLesser >;
  /**
   * return the number of item of type «T»
   */
  template <class T>
  inline size_t size() const {
    return count_if(index.begin(),index.end(),[](const Index::value_type &value) {return dynamic_cast<T*>(value.get())!=nullptr;});
  }

// private:
  Index index;
  const path source;

  /**
   * this class represents Document Items grouped by topic «T»
   */
  template <class T>
  class Topic {
  public:
    // sort is inherited by the underlaying items
    using ItemList=std::vector<T*>;
    Topic(
      const Document  &doc,
      const path      &doc_path,
      const char      *title        = nullptr,
      int              cardinality  = -1
    ) : title(title?title:""),cardinality(cardinality) {
      for_each(
        doc.index.begin(),
        doc.index.end(),
        [this, &doc_path] (const typename decltype(doc.index)::value_type &value) {
          if (auto element = dynamic_cast<T*>(value.get()); element) {
            element->uri  = doc_path;
            items.emplace_back(element);
          }
        }
      );
      if (cardinality>-1 && items.size()>cardinality)
        throw std::domain_error("Wrong cardinality: expected "+std::to_string(cardinality)+" got "+std::to_string(items.size()));
    }
    const std::string title;
    ItemList          items;
    // -1 means any
    const int         cardinality;
  };

  template <class T>
  class Header : public Topic<T> {
  public:
    Header(const Document &doc,const std::filesystem::path &doc_path) : Topic<T>(doc,doc_path,nullptr,1) {}
    explicit operator T* () {return this->items[0];}
  };

};
using DocumentList = std::vector<std::unique_ptr<Document>>;

namespace doc::toc {

/**
 * build an index title in the form
 *
 * "«item name» («item type»)"
 */
inline std::string title(const Item &item) {
  return item.name+" ("+item.type()+')';
}

// copy Document items into the Table of Contents
inline void add(const Document *document, ToC &toc) {
  for(auto &value: document->index)
    toc.emplace(value->indexKey(),value.get());
}

/**
 * returns a «func()» filtered SubToC
 */
template <typename FUNC>
SubToC filter(const std::filesystem::path &path,const ToC &toc, FUNC func) {
  SubToC result;
  for(auto& [key, value]: toc) {
    if (func(path,value))
      result.emplace(key,value);
  }
  return result;
}

} // namespace doc::toc

} // namespace orthodocs
