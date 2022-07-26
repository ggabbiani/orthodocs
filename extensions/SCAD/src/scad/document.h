#pragma once
/*
 * Abstract document declarations
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

#include "doc/license.h"

#include <document.h>
#include <utils.h>

#include <functional>
#include <filesystem>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <string>
#include <vector>

namespace scad::doc {

class Function : public ::doc::Item {
public:
  static const std::string &ID;
  Function(const ::doc::Name &name,bool nested=false);
  ::doc::Signature signature() const {return _signature();}
};

class Module : public ::doc::Item {
public:
  static const std::string &ID;
  Module(const ::doc::Name &name,bool nested=false);
  ::doc::Signature signature() const {return _signature();}
};

/**
 * manages OpenSCAD packages
 *
 * NOTE: Item::name is set to the abstract package path, i.e. the relative
 * path of the package from the source or document root without extension.
 */
class Package : public ::doc::Item {
public:
  static const std::string &ID;
  explicit Package(const std::filesystem::path &path);

  /**
   * return the Packages Index key from its path.
   *
   * «package stem» (package)
   */
  std::string tocKey() const override;

  /**
   * return the Package Index key from a path.
   * This method is used for searching packages from the Package::include and
   * Package::use member values.
   *
   * «package stem» (package)
   */
  static std::string tocKey(const std::string &s);

  /**
   * contains the full abstract package path: root relative path comprehensive of extension
   */
  std::filesystem::path path;
  std::set<std::string,std::less<>> uses;
  std::set<std::string,std::less<>> includes;
  // eventual license
  const char *license = nullptr;
};

class Variable : public ::doc::Item {
public:
  static const std::string &ID;
  Variable(const ::doc::Name &name,const ::doc::Value &defaults,bool nested=false);
};

/**
 * base for comment styles.
 * FIXME: comment style management work on parsed text, as such it should be
 * outside source specific coding.
 */
struct AbstractStyle {
  virtual ~AbstractStyle() = default;
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
  static const std::array<const char *,2> decoration;
};

class Fine : public AbstractStyle {
  friend class Factory;

  static constexpr const char *ID = "FINE";
  enum Decoration {start,end,body};
  bool check(const std::string &text) override;
  std::string manage(const std::string &text) override;
  const char *id() override;

  int column;
  static const std::array<const char *,3> decoration;
};

} // namespace style

} // namespace scad::doc
