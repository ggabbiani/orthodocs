/*
 * Abstract document definition
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

#include "scad/document.h"
#include "orthodocs/globals.h"
#include "orthodocs/utils.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <string.h>
#include <sstream> 

using namespace std;

namespace fs = std::filesystem;

namespace scad {
  
namespace doc {

string Package::indexKey(const string &s) {
  return fs::path(s).stem().string()+" ("+ID+")";
}

string Package::indexKey() const {
  auto    len   = Option::ignore_prefix().length();
  auto    stem  = path.stem().string();
  string  s     = len==0 || !nocase::compare(stem.substr(0,len),Option::ignore_prefix()) ? stem : stem.substr(len);
  auto    res   = s+" ("+type()+')';
  return res;
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
  return (lines(text)==1 && text.length()>=3);
}

string Single::manage(const string &text) {
  assert(lines(text)==1);
  assert(text.length()>=3);
  return text.length()==3 ? string() : text.substr(text.find_first_not_of(' ',3));
}

array<const char *,2> Simple::decoration{
  "/*!",
   "*/"
};

bool Simple::check(const string &text) {
  auto len = lines(text);
  if (len<3)
    return false;
  
  stringstream ss(text,ios_base::in);
  auto i        = 1;
  bool goon     = true;
  this->column  = -1;
  while (goon && ss.getline(buffer,sizeof buffer)) {
    const char *m = i==1 ? decoration[start] : i==len ? decoration[end] : nullptr;
    if (m) {
      auto p = strstr(buffer, m);
      goon = (i==1 ? p==buffer : p==buffer+strnlen(buffer,sizeof buffer)-strlen(m));
    } else {
      this->column  = this->column==-1 ? (int)strspn(buffer," ") : min((int)strspn(buffer," "),this->column);
    }
    ++i;
  }
  
  return goon;
}

string Simple::manage(const string &text) {
  auto len = lines(text);
  assert(len>2);
  
  string result;
  stringstream ss(text,ios_base::in);
  auto i    = 1;
  bool goon = true;
  while (goon && ss.getline(buffer,sizeof buffer)) {
    if (i>1 && i<len) 
      result.append(string(buffer).substr(this->column)+'\n');
    ++i;
  }
  return result;
}

const char *Simple::id() {
  return Simple::ID;
}

array<const char *,3> Fine::decoration{
  "/*!",
  " */",
  " *"
};

  /**
   * @brief 
   * 
   * @param text 
   * @return true 
   * @return false 
   */
bool Fine::check(const string &text) {
  auto len = lines(text);
  if (len<3)
    return false;
  
  stringstream ss(text,ios_base::in);
  auto row        = 1;
  bool goon     = true;
  this->column  = -1;
  while (goon && ss.getline(buffer,sizeof buffer)) {
    const char *m = row==1 ? decoration[start] : row==len ? decoration[end] : decoration[body];
    auto pos = strstr(buffer, m);
    if (row==1) {         // parser strip any trailing space on first row
      goon  = pos==buffer;
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
      result.append(string(buffer+offset+(offset<strlen(buffer)))+'\n');
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
  static Simple simple;

  if (fine.check(text))
    return static_cast<AbstractStyle*>(&fine);
  else if (simple.check(text))
    return static_cast<AbstractStyle*>(&simple);
  else if (single.check(text))
    return static_cast<AbstractStyle*>(&single);
  else
    throw runtime_error("**Unrecognized comment type**");
}

}

} // namespace doc

} // namespace scad
