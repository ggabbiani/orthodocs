#include "formatters.h"

#include <sstream>

using namespace std;

namespace doc {

void Formatter::item(const doc::Item& i) {
  if (i.type=="function")
    function(i);
  else if (i.type=="module")
    module(i);
  else if (i.type=="package")
    package(i);
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
  out << "# " << pkg.type << ' ' << pkg.name << endl
      << endl;
  if (!pkg.annotation.empty())
    out << pkg.annotation << endl
        << endl;
}

void Mdown::parameter(const doc::Parameter &p) {
  // out << "* **" << p.name << "**" << ": " << p.annotation << endl;
  out << "  " << p.name << " - " << p.annotation << "  " << endl;
}

void Mdown::function(const doc::Item &func) {
  out << "---" << endl
      << endl
      << "### " << func.type << ' ' << func.name << endl
      << endl
      << "#### Syntax:" << endl
      << endl
      << "`" << signature(func) << "`" << endl
      << endl;
  if (!func.annotation.empty())
    out << func.annotation << endl
        << endl;

  if (func.parameters.size()) {
    out << "#### Parameters:" << endl
        << endl;
    for(auto i=func.parameters.begin();i!=func.parameters.end();i++)
      parameter(**i);
    out << endl;
  }
}

void Mdown::module(const doc::Item &mod) {
  out << "---" << endl
      << endl
      << "### " << mod.type << ' ' << mod.name << endl
      << endl
      << "#### Syntax:" << endl
      << endl
      << "`" << signature(mod) << "`" << endl
      << endl;
  if (!mod.annotation.empty())
    out << mod.annotation << endl
        << endl;

  if (mod.parameters.size()) {
    out << "#### Parameters:" << endl
        << endl;
    for(auto i=mod.parameters.begin();i!=mod.parameters.end();i++)
      parameter(**i);
    out << endl;
  }
}

void Mdown::format(const doc::ItemMap &items) {
  for (auto i=items.begin(); i!=items.end(); ++i) {
    auto itm = i->second.get();
    if (itm->type=="package")
      item(*itm);
  }
  out << "## Functions" << endl << endl;
  for (auto i=items.begin(); i!=items.end(); ++i) {
    auto func = i->second.get();
    if (func->type=="function")
      item(*func);
  }
  out << "## Modules" << endl << endl;
  for (auto i=items.begin(); i!=items.end(); ++i) {
    auto mod = i->second.get();
    if (mod->type=="module")
      item(*mod);
  }
}

}

}
