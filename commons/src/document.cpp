/*
 * synthetic document implementation
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <commons/document.h>
#include <commons/globals.h>

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
  return !cli::private_prefix().empty() && name.substr(0,cli::private_prefix().length())==cli::private_prefix();
}

string Item::tocKey() const {
  return cli::prefixAbbreviation(name)+" ("+type+')';
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
