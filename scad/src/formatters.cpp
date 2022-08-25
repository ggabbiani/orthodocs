#include "formatters.h"
#include "utils.h"

#include <sstream>

using namespace std;

namespace doc {

size_t size(const ItemMap &items,const type_info &type) {
  auto size  = 0;
  for(auto i=items.begin(); i!=items.end(); ++i) {
    auto var  = i->second.get();
    size += (typeid(*var)==type);
  }
  return size;
}

namespace formatter {

Mdown::Mdown(ostream &out) : out(out) {}

void Mdown::package(const doc::Package &pkg) {
  out << H("package "+pkg.name) << endl
      << endl;
  if (!pkg.annotation.empty())
    out << pkg.annotation << endl
        << endl;
}

void Mdown::parameter(const doc::Parameter &p) {
  out << BOLD(p.name()) << BR()
      << p.annotation() << endl
      << endl;
}

void Mdown::variable(const doc::Variable &var) {
  out << HRULE()
      << H("variable "+var.name,3)
      << endl;
  if (!var.defaults.empty())
    out << BOLD("Default:") << endl
        << endl
        << "    " << var.defaults << endl
        << endl;
  if (!var.annotation.empty())
    out << var.annotation << endl
        << endl;
}

void Mdown::function(const doc::Function &func) {
  out << HRULE()
      << H("function "+func.name,3)
      << endl
      << BOLD("Syntax:") << endl
      << endl
      // << CODE(signature(func)) << endl
      << "    " << func.signature() << endl
      << endl;
  if (!func.annotation.empty())
    out << func.annotation << endl
        << endl;

  if (func.parameters.size()) {
    // how many annotated parameters do we have in place?
    auto annotations  = 0;
    for(auto i=func.parameters.begin();i!=func.parameters.end();++i) {
      annotations +=(!(*i)->annotation().empty());
    }
    if (annotations) {
      out << BOLD("Parameters:") << endl
          << endl;
      for(auto i=func.parameters.begin();i!=func.parameters.end();i++) {
        auto p = **i;
        if (!p.annotation().empty())
          parameter(**i);
      }
      out << endl;
    }
  }
}

void Mdown::module(const doc::Module &mod) {
  out << HRULE()
      << H("module "+mod.name,3)
      << endl
      << BOLD("Syntax:") << endl
      << endl
      // << CODE(signature(mod)) << endl
      << "    " << mod.signature() << endl
      << endl;
  if (!mod.annotation.empty())
    out << mod.annotation << endl
        << endl;

  if (mod.parameters.size()) {
    // how many annotated parameters do we have in place?
    auto annotations  = 0;
    for(auto i=mod.parameters.begin();i!=mod.parameters.end();++i) {
      annotations +=(!(*i)->annotation().empty());
    }
    if (annotations) {
      out << BOLD("Parameters:") << endl
          << endl;
      for(auto i=mod.parameters.begin();i!=mod.parameters.end();i++) {
        auto p = **i;
        if (!p.annotation().empty())
          parameter(**i);
      }
      out << endl;
    }
  }
}

void Mdown::format(const doc::ItemMap &items) {

  for (auto i=items.begin(); i!=items.end(); ++i) {
    auto pkg = i->second.get();
    if (is<Package>(*pkg))
      package(dynamic_cast<const Package&>(*pkg));
  }

  if (size(items,typeid(Variable))) {
    out << H("Variables",2) << endl 
        << endl;
    for (auto i=items.begin(); i!=items.end(); ++i) {
      auto var  = i->second.get();
      if (is<Variable>(*var))
        variable(dynamic_cast<const Variable&>(*var));
    }
  }

  if (size(items,typeid(Function))) {
    out << H("Functions",2) << endl 
        << endl;
    for (auto i=items.begin(); i!=items.end(); ++i) {
      auto func = i->second.get();
      if (is<Function>(*func))
        function(dynamic_cast<const Function&>(*func));
    }
  }

  if (size(items,typeid(Module))) {
    out << H("Modules",2) << endl 
        << endl;
    for (auto i=items.begin(); i!=items.end(); ++i) {
      auto mod = i->second.get();
      if (is<Module>(*mod))
        module(dynamic_cast<const Module&>(*mod));
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
