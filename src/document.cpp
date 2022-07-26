/*
 * synthetic document implementation
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

#include "document.h"
#include "globals.h"

using namespace std;
namespace fs=std::filesystem;

namespace doc {

Item::Item(const Name &name,const std::string &type, const Value *defaults,bool nested)
  : name(name),defaults(defaults?*defaults:""),nested(nested),type(type) {
}

string Item::documentKey() const {
  return type+' '+name;
}

bool Item::privateId() const {
  return !Option::private_prefix().empty() && name.substr(0,Option::private_prefix().length())==Option::private_prefix();
}

string Item::tocKey() const {
  return Option::prefix_abbreviation(name)+" ("+type+')';
}

Signature Item::_signature() const {
  ostringstream ss;
  ss << name << "(";
  for(auto i=parameters.begin();i!=parameters.end();i++) {
    if (i!=parameters.begin())
      ss << ",";
    const Parameter *parameter = i->get();
    ss << parameter->name;
    if (!parameter->defaults.empty())
      ss << "=" << parameter->defaults;
  }
  ss << ")";
  return ss.str();
}

} // namespace doc
