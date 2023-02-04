/*
 * Abstract document definition
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <scad/document.h>
#include <scad/extension.h>

#include <commons/globals.h>
#include <commons/utils.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <filesystem>
#include <string.h>
#include <sstream>

#include <iostream>

using namespace std;

namespace fs = std::filesystem;

namespace scad::doc {

/******************************************************************************
 * Function
 *****************************************************************************/
const string &Function::ID  = ::scad::Extension::slot[::scad::Extension::TYPE_FUNCTION].type;

Function::Function(const ::doc::Name &name,bool nested) : ::doc::Item(name,ID,nullptr,nested) {
  dictKey = ::scad::Extension::slot[::scad::Extension::TYPE_FUNCTION].dictionaryKey(this);
}

/******************************************************************************
 * Module
 *****************************************************************************/
const string &Module::ID    = ::scad::Extension::slot[::scad::Extension::TYPE_MODULE].type;

Module::Module(const ::doc::Name &name,bool nested) : ::doc::Item(name,ID,nullptr,nested) {
  dictKey = ::scad::Extension::slot[::scad::Extension::TYPE_MODULE].dictionaryKey(this);
}

/******************************************************************************
 * Package
 *****************************************************************************/
const string &Package::ID   = ::scad::Extension::slot[::scad::Extension::TYPE_PACKAGE].type;

Package::Package(const std::filesystem::path &path)
  : ::doc::Item((path.parent_path()/path.stem()).string(),ID,nullptr,false), path(path) {
  dictKey = ::scad::Extension::slot[::scad::Extension::TYPE_PACKAGE].dictionaryKey(this);
}

string Package::tocKey(const string &s) {
  return fs::path(s).stem().string()+" ("+ID+")";
}

string Package::tocKey() const {
  return Option::prefix_abbreviation(path.stem().string())+" ("+type+')';
}

/******************************************************************************
 * Variable
 *****************************************************************************/
const string &Variable::ID  = ::scad::Extension::slot[::scad::Extension::TYPE_VARIABLE].type;

Variable::Variable(const ::doc::Name &name,const ::doc::Value &defaults,bool nested)
: ::doc::Item(name,ID,&defaults,nested) {
  dictKey = ::scad::Extension::slot[::scad::Extension::TYPE_VARIABLE].dictionaryKey(this);
}


/******************************************************************************
 * Comment styles
 *****************************************************************************/
namespace style {

static char buffer[256];

string Abstract::manage(const string &text) {
  if (!check(text))
    throw BadStyle();
  return data();
}

const char *Single::id() {
  return Single::ID;
}

bool Single::check(const string &text) {
  // text in caption group #1
  static regex re{R"(//)"+Option::decorations()+R"(\s((.|\n)*))"};
  smatch match;
  auto success = regex_search(text,match,re);
  if (success)
    setData(match[1].str());
  return success;
}

bool Fine::check(const string &text) {
  // text in caption group #2
  // Option::decorations()=="@" ⟹ (/\*@\s*\n)((\s*\*\s.*\n)*)(\s*\*/)
  // Option::decorations()==""  ⟹ (/\*\s*\n)((\s*\*\s.*\n)*)(\s*\*/)
  static regex re{R"((/\*)"+Option::decorations()+R"(\s*\n)((\s*\*\s.*\n)*)(\s*\*/))"};
  smatch match;
  auto success  = regex_search(text,match,re);
  if (success) {
    stringstream ss(match[2],ios_base::in);
    this->column  = -1;
    while (success && ss.getline(buffer,sizeof buffer)) {
      auto pos  = strstr(buffer, " *");
      success      = pos!=nullptr;
      if (success) {
        if (this->column==-1) {
          // column uninitialized ⟹ first resulting line
          this->column  = (int)(pos-buffer+1);
        } else {
          // new column equal to the previous one?
          success  = (pos-buffer+1==this->column);
        }
      }
    }
  }
  if (success)
    setData(match[2].str());
  return success;
}

string Fine::manage(const string &text) {
  if (!check(text))
    throw BadStyle();

  stringstream ss(data(),ios_base::in);
  string result;
  while (ss.getline(buffer,sizeof buffer)) {
    auto offset = this->column+1;
    result.append(string(buffer+offset+(int)(offset<strnlen(buffer,sizeof buffer)))+'\n');
  }
  return result;
}

const char *Fine::id() {
  return Fine::ID;
}

const char *Block::id() {
  return Block::ID;
}

bool Block::check(const string &text) {
  // text in caption group #2
  // Option::decorations()=="@" ⇒ (/\*@)((.|\n)*)(\*/)
  // Option::decorations()==""  ⇒ (/\*)((.|\n)*)(\*/)
  static std::regex re{R"((/\*)"+Option::decorations()+R"()((.|\n)*)(\*/))"};
  smatch match;
  auto success = regex_search(text,match,re);
  if (success)
    setData(match[2].str());
  return success;
}

Abstract *Factory::operator()(const string &text) {
  static Single single;
  static Fine   fine;
  static Block  block;

  if (fine.check(text))
    return static_cast<Abstract*>(&fine);
  else if (single.check(text))
    return static_cast<Abstract*>(&single);
  else if (block.check(text))
    return static_cast<Abstract*>(&block);
  else
    return nullptr;
}

Abstract::BadStyle::BadStyle()
  : std::runtime_error("Style result access after unsuccessful match.") {
}

} // namespace style

} // namespace scad::doc
