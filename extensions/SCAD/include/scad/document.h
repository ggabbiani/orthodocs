#pragma once
/*
 * Abstract document declarations
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <commons/document.h>
#include <commons/utils.h>

#include <functional>
#include <filesystem>
#include <map>
#include <memory>
#include <regex>
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
  using Owner = std::unique_ptr<Package>;

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
};

class Variable : public ::doc::Item {
public:
  static const std::string &ID;
  Variable(const ::doc::Name &name,const ::doc::Value &defaults,bool nested=false);
};

namespace style {

/**
 * Abstract comment style.
 */
class Abstract {
public:
  /**
   * Trying to manage a style in failed state
   */
  struct BadStyle : std::runtime_error {
    BadStyle();
  };
  /**
   * Returns text from comment.
   *
   * Text is check()ed, in case of failure BadStyle is thrown.
   */
  virtual std::string manage(const std::string &text);
  /**
   * Enables polymorphic behaviour on derived destructors.
   */
  virtual ~Abstract() = default;
  /**
   * Returns true when the passed «text» is in style
   */
  virtual bool check(const std::string &text) = 0;
  /**
   * Returns the style ID of which the object is an instance.
   */
  virtual const char *id() = 0;
protected:
  // data getter
  std::string &data() {return _data;}
  // data setter
  void setData(std::string_view data) {_data = data;}
private:
  std::string _data;
};

class Factory {
public:
  /**
   * return the recognized style or null
   */
  Abstract * operator () (const std::string &text);
};

/**
 * Single style: c++ single line comment
 */
class Single : public Abstract {
  friend class Factory;
public:
  static constexpr const char *ID = "SINGLE";
private:
  bool check(const std::string &text) override;
  const char *id() override;
};

/**
 * Fine style: c-like block comment WITH internal alignment
 */
class Fine : public Abstract {
  friend class Factory;
public:
  static constexpr const char *ID = "FINE";
private:
  bool check(const std::string &text) override;
  std::string manage(const std::string &text) override;
  const char *id() override;

  int         column;
};

/**
 * Block style: c-like block comment with no internal alignment
 */
class Block : public Abstract {
  friend class Factory;
public:
  static constexpr const char *ID = "block";
private:
  bool check(const std::string &text) override;
  const char *id() override;
};
} // namespace style

} // namespace scad::doc
