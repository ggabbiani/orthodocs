/*
 * Abstract document definitions
 *
 * Copyright © 2022 Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * This file is part of the 'AutoDox' (ADOX) project.
 *
 * ADOX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ADOX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ADOX.  If not, see <http: //www.gnu.org/licenses/>.
 */

#pragma once

#include "utils.h"

#include <filesystem>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <string>
#include <vector>

class Index;

namespace scad {

class Listener;

}

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
  friend class ::scad::Listener;
public:
  const Name &name() const {return _name;}
  const Annotation &annotation() const {return _annotation;}
  Value       defaults;
protected:
  Name        _name;
  Annotation  _annotation;
};
using ParameterPtr  = std::unique_ptr<Parameter>;
using ParameterVec  = std::vector<ParameterPtr>;

class Item {
  friend class ::Index;
public:
  virtual ~Item() = default;

  virtual std::string type() const = 0;

  /**
   * builds key value usable by Document
   */
  virtual std::string documentKey() const;

  /**
   * builds an key value usable by Index.
   * 
   * «item name» («item type»)
   */
  virtual std::string indexKey() const;


  Name          name;
  Annotation    annotation;
  ParameterVec  parameters;
  const Value   defaults;
  const bool    nested;
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

}

/**
 * Document format:
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
using Document = std::map<std::string,doc::ItemPtr>;

namespace doc {

class Variable : public Item {
public:
  Variable(const Name &name,const Value &defaults,bool nested=false) : Item(name,&defaults,nested) {}
  std::string type() const override {return "variable";}
};

class Function : public Item {
public:
  Function(const Name &name,bool nested=false) : Item(name,nullptr,nested) {}
  Signature signature() const {return Item::signature();}
  std::string type() const override {return "function";}
};

class Module : public Item {
public:
  Module(const Name &name,bool nested=false) : Item(name,nullptr,nested) {}
  Signature signature() const {return Item::signature();}
  std::string type() const override {return "module";}
};

class Package : public Item {
public:
  /**
   * Construct a new Package object
   * 
   * NOTE: Item::name is equal to the abstract package path
   */
  Package(const std::filesystem::path &path) : Item(path.parent_path()/path.stem(),nullptr,false), path(path) {}
  /**
   * always return "package"
   */
  std::string type() const override {return "package";}
  /**
   * Packages provide different keys for Index, building it from the package 
   * abstract path.
   * 
   * «package name» (package)
   */
  std::string indexKey() const override;
  /**
   * Builds a Package key for Index from a package abstract path.
   * This method is used for searching packages from the Package::include and 
   * Package::use member values.
   * 
   * «package name» (package)
   */
  static std::string indexKey(const std::string &s);

  /**
   * full package path comprehensive of extension
   */
  std::filesystem::path path;
  std::set<std::string> uses;
  std::set<std::string> includes;
};

struct AbstractStyle {
  virtual bool check(const std::string &text) = 0;
  virtual std::string manage(const std::string &text) = 0;
  virtual const char *id() = 0;
};

namespace style {

class Factory {
public:
  AbstractStyle * operator () (const std::string &text);
};

class Single : public AbstractStyle {
  friend class Factory;

  static constexpr const char *ID = "SINGLE";
  bool check(const std::string &text) override;
  std::string manage(const std::string &text) override;
  const char *id() override;
};

class Simple : public AbstractStyle {
  friend class Factory;

  static constexpr const char *ID = "SIMPLE";
  enum Decoration {start,end};
  bool check(const std::string &text) override;
  std::string manage(const std::string &text) override;
  const char *id() override;

  int column;
  static const char *decoration[2];
};

class Fine : public AbstractStyle {
  friend class Factory;

  static constexpr const char *ID = "FINE";
  enum Decoration {start,end,body};
  bool check(const std::string &text) override;
  std::string manage(const std::string &text) override;
  const char *id() override;

  int column;
  static const char *decoration[3];
};

}

using ToC = std::multimap<Name,ItemPtr,nocase::Compare>;

namespace toc {

/**
 * build an index title in the form
 * 
 * "«item name» («item type»)"
 */
inline std::string title(const Item &item) {
  return item.name+" ("+item.type()+')';
}

inline void insert(ToC::mapped_type &item, ToC &map) {
  map.emplace(item->indexKey(),std::move(item));
}

// move Document items into index
inline void add(Document &document, ToC &map) {
  for(auto &item: document) 
    insert(item.second,map);
}

}

}
