/*
 * abstract document implementation
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

#include "orthodocs/document.h"
#include "orthodocs/globals.h"

using namespace std;
namespace fs=std::filesystem;

namespace orthodocs {

namespace doc {

string Item::documentKey() const {
  return type()+' '+name;
}

string Item::indexKey() const {
  auto len  = Option::prefix().length();
  string s  = len==0 || !nocase::compare(name.substr(0,len),Option::prefix()) ? name : name.substr(len);
  auto res  = s+" ("+type()+')';
  return res;
}

Signature Item::signature() const {
  ostringstream ss;
  ss << name << "(";
  for(auto i=parameters.begin();i!=parameters.end();i++) {
    if (i!=parameters.begin())
      ss << ",";
    auto parameter = i->get();
    ss << parameter->name();
    if (!parameter->defaults.empty())
      ss << "=" << parameter->defaults;
  }
  ss << ")";
  return ss.str();
}

namespace toc {

SubToC filter(const fs::path &path,const ToC &toc, function<bool(const fs::path&,const Item*)> func) {
  SubToC result;
  for(auto& [key, value]: toc) {
    if (func(path,value))
      result.emplace(key,value);
  }
  return result;
}

} // namespace toc

} // namespace doc

} // namespace orthodocs
