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

#include <debug/trace.h>

namespace language {

class Extension;

} // namespace language

namespace orthodocs {

namespace doc {

class Item;

/**
 * Simple cross-reference namespace.
 * 
 * The algorithm used is based on three elements:
 * 
 * 1. a language extension for reference analysis; 
 * 2. one dictionary, containing all the referrable items;
 * 3. a writer extension for references substitution during document saving.
 */
namespace xref {

struct Analysis {
  using Results = std::map<size_t,Analysis>;
  using Owner   = std::unique_ptr<Results>;
  // token position as a delta from the start of the annotation's string
  ptrdiff_t   position;
  // resulting token length to be substituted when resolving the reference
  ptrdiff_t   length;
  // to be searched to in the dictionary
  std::string token;
};

/**
 * CASE SENSITIVE Item comparison functor.
 */
struct DictLess {
  bool operator() (const Item *lhs, const Item *rhs) const;
};

/**
 * Through the contained doc::Items is realized a map between two domains:
 * - language domain for the referred token (see DictLess and Item::dictKey());
 * - writer domain for the concrete reference (see writer::Extension::reference())
 */
using Dictionary  = std::map< std::string, orthodocs::doc::Item* >;

}

struct Annotation {
  bool empty() const {return data.empty();}

  std::string data;
  xref::Analysis::Results results;
};


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
public:
  using Owner = std::unique_ptr<Item>;
  // used by generators for stackable items (modules and variables)
  using OwnerStack  = std::stack<Owner>;

  /**
   * CASE SENSITIVE Document::Index comparison functor.
   * NOTE: see doc::Item::documentKey() for the format
   */
  struct Less {
    inline bool operator() (const Owner &lhs, const Owner &rhs) const {
      return  lhs->documentKey() < rhs->documentKey();
    }
  };

  /**
   * CASE INSENSITIVE doc::ToC comparison functor.
   * NOTE: see doc::Item::documentKey() for the format
   */
  struct LessNoCase {
    using is_transparent = void; // enables heterogeneous lookup
    bool operator() (const Item *lhs, const Item *rhs) const {
      return nocase::Less()(lhs->tocKey(),rhs->tocKey());
    }
  };

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
   * builds a key value usable by doc::ToC.
   *
   * «item name» («item type»)
   */
  virtual std::string tocKey() const;

  /**
   * see the concrete language implementation for the actual format
   */
  virtual std::string dictKey() const = 0;

  Name            name;
  Annotation      annotation;
  Parameter::Vec  parameters;
  const Value     defaults;
  const bool      nested;

  /**
   * this field is filled by the language extension
   */
  Item          *parent = nullptr;

protected:
  Item(const Name &name,const Value *defaults=nullptr,bool nested=false) : name(name),defaults(defaults?*defaults:""),nested(nested) {}
  Signature _signature() const;

};

/**
 * Table of Contents with no ownership on represented items.
 * 
 * NOTE: the key is possibly abbreviated, in this case a collision is accepted
 */
using ToC = std::multiset<Item*,doc::Item::LessNoCase>;

/**
 * SubToC is a filtered selection of ToC elements, as such its items are
 * simple pointers, because a SubToC has no ownerships over them.
 * It is critical that SubToC never survive its ToC.
 */
using SubToC = ToC;

} // namespace doc

class Document {
  using path  = std::filesystem::path;
public:
  using Owner = std::unique_ptr<Document>;
  using Index = std::set<doc::Item::Owner, doc::Item::Less>;

  explicit Document(const path &source) : source(source) {}

  /**
   * return the number of item of type «T»
   */
  template <class T>
  inline size_t size() const {
    return count_if(index.begin(),index.end(),[](const Index::value_type &value) {return dynamic_cast<T*>(value.get())!=nullptr;});
  }

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
          if (auto element = dynamic_cast<T*>(value.get()); element) 
            items.emplace_back(element);
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
    // TODO: could we render the operator implicit?
    explicit operator T* () {return this->items[0];}
  };

  Index index;
  const path source;
};
using DocumentList = std::vector<std::unique_ptr<Document>>;

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
inline void add(const Document *document, ToC &toc) {
  for(auto &value: document->index)
    toc.emplace(value.get());
}

/**
 * returns a «func()» filtered SubToC
 */
template <typename FUNC>
SubToC filter(const std::filesystem::path &path,const ToC &toc, FUNC func) {
  SubToC result;
  for(auto value: toc) {
    if (func(path,value))
      result.emplace(value);
  }
  return result;
}

} // namespace toc

namespace xref {

inline bool DictLess::operator() (const Item *lhs, const Item *rhs) const {
  return  lhs->dictKey() < rhs->dictKey();
}

}

} // namespace doc

} // namespace orthodocs
