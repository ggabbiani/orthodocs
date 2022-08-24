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

struct Parameter {
  std::string name;
  std::string annotation;
  std::string defaults;
};
using ParameterPtr  = std::unique_ptr<Parameter>;
using ParameterVec  = std::vector<ParameterPtr>;

struct Item {
  enum Type {
    function,module,package,variable
  };
  Item(const std::string &name,Type type,bool nested=false) : name(name),type(type),nested(nested) {}
  std::string   name;
  std::string   annotation;
  Type          type;
  ParameterVec  parameters;
  std::string   defaults;
  bool nested;
};
using ItemPtr = std::unique_ptr<Item>;
using ItemMap = std::map<std::string,ItemPtr>;

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
