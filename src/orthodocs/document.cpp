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
 * along with ODOX.  If not, see <http: //www.gnu.org/licenses/>.
 */

#include "orthodocs/document.h"
#include "orthodocs/globals.h"

using namespace std;
namespace fs=std::filesystem;

namespace orthodocs {

namespace doc {

size_t size(const Document *items,const type_info &type) {
  auto size  = 0;
  for(auto i=items->begin(); i!=items->end(); ++i) {
    auto var  = i->second.get();
    size += (typeid(*var)==type);
  }
  return size;
}

string Item::documentKey() const {
  return type()+' '+name;
}

string Item::indexKey() const {
  auto len  = option::prefix.length();
  string s  = option::prefix.empty() || !nocase::compare(name.substr(0,len),option::prefix) ? name : name.substr(len);
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
  for(auto &element: toc) {
    auto item = element.second.get();
    if (func(path,item))
      result.emplace(element.first,item);
  }
  return result;
}

} // namespace toc

} // namespace doc

} // namespace orthodocs
