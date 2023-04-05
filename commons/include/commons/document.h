#pragma once
/*
 * synthetic document
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <commons/annotation.h>
#include <commons/error_info.h>
#include <commons/utils.h>

#include <cassert>
#include <filesystem>
#include <functional>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>

namespace language {

class Extension;

} // namespace language

namespace doc {

class Item;

/**
 * Simple cross-reference namespace based on three elements:
 *
 * 1. a language extension for reference analysis;
 * 2. one dictionary, containing all the referencible items;
 * 3. a writer extension for references substitution during document saving.
 */
namespace xref {

struct Analysis : public analytic::Data {
  Analysis(Position pos, Size len,const std::string &tok, const std::string &lit)
  : analytic::Data{pos,len}, token{tok}, literal{lit} {}

  // token to be searched for in the inclusion dictionary
  std::string token;
  // literal to be searched for in the exclusion vocabulary
  std::string literal;
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
using Dictionary  = std::map< std::string, ::doc::Item*, std::less<> >;
using Vocabulary  = std::set< std::string, std::less<> >;

}

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
  using Owner  = std::unique_ptr<Parameter>;
  using OwnerVec  = std::vector<Owner>;

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

  Name                name;
  Annotation          annotation;
  Parameter::OwnerVec parameters;
  const Value         defaults;
  const bool          nested;
  const std::string  &type;
  std::string         dictKey;

  /**
   * this field is filled by the language extension
   */
  Item          *parent = nullptr;

protected:
  Item(const Name &name,const std::string &type, const Value *defaults=nullptr,bool nested=false);
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
  using Index = std::set<doc::Item::Owner, doc::Item::Less>;
  using Id    = std::size_t;
  using Owner = std::unique_ptr<Document>;
  using List  = std::vector<Owner>;

  /*
   * used by document graph
   */
  struct Vertex {
    const Document *document;
  };

  /*
   * used by document graph
   */
  struct Edge {
    /**
     * can be "use" or "include" eventually containing the stereotype
     */
    const std::string type;
  };

  Document(const path &source, int id) : source{source}, id{id} {}

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
      const char      *title        = nullptr,
      int              cardinality  = -1
    ) : title(title?title:""),cardinality(cardinality) {
      for_each(
        doc.index.begin(),
        doc.index.end(),
        [this] (const typename decltype(doc.index)::value_type &value) {
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
    explicit Header(const Document &doc) : Topic<T>(doc,nullptr,1) {}
    explicit operator T* () {return this->items[0];}
  };

  Index       index;
  const path  source;
  const int   id;
};

namespace doc {

namespace toc {

/**
 * build an index title in the form
 *
 * "«item name» («item type»)"
 */
inline std::string title(const Item &item) {
  return item.name+" ("+item.type+')';
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
  return  lhs->dictKey < rhs->dictKey;
}

}

} // namespace doc
