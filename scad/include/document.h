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

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace doc {

using Annotation  = std::string;
using Name        = std::string;
using Value       = std::string;

struct Parameter {
  Name        name;
  Annotation  annotation;
  Value       defaults;
};
using ParameterPtr  = std::unique_ptr<Parameter>;
using ParameterVec  = std::vector<ParameterPtr>;

class Item {
public:
  virtual ~Item() = default;
  Name          name;
  Annotation    annotation;
  ParameterVec  parameters;
  const Value   defaults;
  const bool    nested;
protected:
  Item(const Name &name,const Value *defaults=nullptr,bool nested=false) : name(name),nested(nested),defaults(defaults?*defaults:"") {}
};
using ItemPtr = std::unique_ptr<Item>;
using ItemMap = std::map<std::string,ItemPtr>;

class Variable : public Item {
public:
  Variable(const Name &name,const Value &defaults,bool nested=false) : Item(name,&defaults,nested) {}
private:
};

class Function : public Item {
public:
  Function(const Name &name,bool nested=false) : Item(name,nullptr,nested) {}
private:
};

class Module : public Item {
public:
  Module(const Name &name,bool nested=false) : Item(name,nullptr,nested) {}
private:
};

class Package : public Item {
public:
  Package(const Name &name) : Item(name,nullptr,false) {}
private:
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

}
