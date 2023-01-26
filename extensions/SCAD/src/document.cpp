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

namespace style {

static char buffer[256];

// calculate the number of lines
static size_t lines(const string &text) {
  stringstream ss(text,ios_base::in);
  auto len = 0;
  while (ss.getline(buffer,sizeof buffer))
    ++len;
  return len;
}

const char *Single::id() {
  return Single::ID;
}

bool Single::check(const string &text) {
  string prolog = "//"+Option::annotationProlog()+" ";
  return (lines(text)==1 && text.find(prolog)==0);
}

string Single::manage(const string &text) {
  assert(lines(text)==1);
  assert(text.length()>=3);
  return text.length()==3 ? string() : text.substr(text.find_first_not_of(' ',3));
}

bool Fine::check(const string &text) {
  const array<string,3> decoration{
    "/*"+Option::annotationProlog(),
    " */",
    " *"
  };

  auto len = lines(text);
  if (len<3)
    return false;

  stringstream ss(text,ios_base::in);
  auto row        = 1;
  bool goon     = true;
  this->column  = -1;
  while (goon && ss.getline(buffer,sizeof buffer)) {
    const char * const m = row==1 ? decoration[start].c_str() : row==len ? decoration[end].c_str() : decoration[body].c_str();
    auto pos = strstr(buffer, m);
    if (row==1) {         // parser strip any trailing space on first row
      goon  = pos==buffer && strlen(buffer)==decoration[start].length();
    } else if (row==2) {       // we take the column number of the '*'
      goon  = pos!=nullptr;
      if (goon)
        this->column  = (int)(pos-buffer+1);
    } else if (row<len) // check if '*' column is equal to the one we got in row 2
      goon  = (pos-buffer+1==this->column);
    ++row;
  }

  return goon;
}

string Fine::manage(const string &text) {
  string result;
  auto len = lines(text);
  assert(len>2);

  stringstream ss(text,ios_base::in);
  auto i    = 1;
  bool goon = true;
  while (goon && ss.getline(buffer,sizeof buffer)) {
    if (i>1 && i<len) {
      auto offset = this->column+1;
      result.append(string(buffer+offset+(int)(offset<strnlen(buffer,sizeof buffer)))+'\n');
    }
    ++i;
  }

  return result;
}

const char *Fine::id() {
  return Fine::ID;
}

AbstractStyle *Factory::operator()(const string &text) {
  static Single single;
  static Fine   fine;
  // static Simple simple;

  if (fine.check(text))
    return static_cast<AbstractStyle*>(&fine);
  // else if (simple.check(text))
  //   return static_cast<AbstractStyle*>(&simple);
  else if (single.check(text))
    return static_cast<AbstractStyle*>(&single);
  else
    return nullptr;
}

}

} // namespace scad::doc
