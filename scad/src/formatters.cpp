#include "formatters.h"

#include <sstream>

using namespace std;

namespace doc {

size_t size(const ItemMap &items,Item::Type type) {
  auto size  = 0;
  for(auto i=items.begin(); i!=items.end(); ++i) {
    auto var  = i->second.get();
    size += (var->type==type);
  }
  return size;
}

void Formatter::item(const doc::Item& i) {
  switch (i.type) {
  case Item::function:
    function(i);
    break;
  case Item::module:
    module(i);
    break;
  case Item::package:
    package(i);
    break;
  case Item::variable:
    variable(i);
    break;
  }
}

string Formatter::signature(const doc::Item &item) {
  ostringstream ss;
  ss << item.name << "(";
  for(auto i=item.parameters.begin();i!=item.parameters.end();i++) {
    if (i!=item.parameters.begin())
      ss << ",";
    auto parameter = i->get();
    ss << parameter->name;
    if (!parameter->defaults.empty())
      ss << "=" << parameter->defaults;
  }
  ss << ")";
  return ss.str();
}

namespace formatter {

Mdown::Mdown(ostream &out) : out(out) {}

void Mdown::package(const doc::Item &pkg) {
  out << H("package "+pkg.name) << endl
      << endl;
  if (!pkg.annotation.empty())
    out << pkg.annotation << endl
        << endl;
}

void Mdown::parameter(const doc::Parameter &p) {
  out << BOLD(p.name) << BR()
      << p.annotation << endl
      << endl;
}

void Mdown::variable(const doc::Item &var) {
  out << HRULE()
      << H("variable "+var.name,3)
      << endl;
  if (!var.defaults.empty())
    out << BOLD("Default:") << ' ' << var.defaults << endl
        << endl;
  if (!var.annotation.empty())
    out << var.annotation << endl
        << endl;
}

void Mdown::function(const doc::Item &func) {
  out << HRULE()
      << H("function "+func.name,3)
      << endl
      << BOLD("Syntax:") << endl
      << endl
      << CODE(signature(func)) << endl
      << endl;
  if (!func.annotation.empty())
    out << func.annotation << endl
        << endl;

  if (func.parameters.size()) {
    // how many annotated parameters do we have in place?
    auto annotations  = 0;
    for(auto i=func.parameters.begin();i!=func.parameters.end();++i) {
      annotations +=(!(*i)->annotation.empty());
    }
    if (annotations) {
      out << BOLD("Parameters:") << endl
          << endl;
      for(auto i=func.parameters.begin();i!=func.parameters.end();i++) {
        auto p = **i;
        if (!p.annotation.empty())
          parameter(**i);
      }
      out << endl;
    }
  }
}

void Mdown::module(const doc::Item &mod) {
  out << HRULE()
      << H("module "+mod.name,3)
      << endl
      << BOLD("Syntax:") << endl
      << endl
      << CODE(signature(mod)) << endl
      << endl;
  if (!mod.annotation.empty())
    out << mod.annotation << endl
        << endl;

  if (mod.parameters.size()) {
    // how many annotated parameters do we have in place?
    auto annotations  = 0;
    for(auto i=mod.parameters.begin();i!=mod.parameters.end();++i) {
      annotations +=(!(*i)->annotation.empty());
    }
    if (annotations) {
      out << BOLD("Parameters:") << endl
          << endl;
      for(auto i=mod.parameters.begin();i!=mod.parameters.end();i++) {
        auto p = **i;
        if (!p.annotation.empty())
          parameter(**i);
      }
      out << endl;
    }
  }
}

void Mdown::format(const doc::ItemMap &items) {

  for (auto i=items.begin(); i!=items.end(); ++i) {
    auto itm = i->second.get();
    if (itm->type==Item::package)
      item(*itm);
  }

  if (size(items,Item::variable)) {
    out << H("Variables",2) << endl 
        << endl;
    for (auto i=items.begin(); i!=items.end(); ++i) {
      auto var  = i->second.get();
      if (var->type==Item::variable)
        item(*var);
    }
  }

  if (size(items,Item::function)) {
    out << H("Functions",2) << endl 
        << endl;
    for (auto i=items.begin(); i!=items.end(); ++i) {
      auto func = i->second.get();
      if (func->type==Item::function)
        item(*func);
    }
  }

  if (size(items,Item::module)) {
    out << H("Modules",2) << endl 
        << endl;
    for (auto i=items.begin(); i!=items.end(); ++i) {
      auto mod = i->second.get();
      if (mod->type==Item::module)
        item(*mod);
    }
  }
}

std::string Mdown::BOLD(const std::string &text) {
  return "__"+text+"__";
}

std::string Mdown::BR() {
  return "  \n";
}

std::string Mdown::H(const std::string &text,int level) {
  string prefix;
  prefix.append(level,'#');
  return prefix + ' ' + text + '\n';
}

std::string Mdown::HRULE() {
  return string("---\n\n");
}

std::string Mdown::CODE(const std::string &text) {
  return "`"+text+"`";
}

}

}
