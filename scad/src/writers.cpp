#include "xref.h"
#include "utils.h"
#include "writers.h"

#include <cassert>
#include <sstream>

using namespace std;
namespace fs  = std::filesystem;

namespace doc {

size_t size(const ItemMap &items,const type_info &type) {
  auto size  = 0;
  for(auto i=items.begin(); i!=items.end(); ++i) {
    auto var  = i->second.get();
    size += (typeid(*var)==type);
  }
  return size;
}

namespace writer {

void Mdown::package(const doc::Package &pkg) {
  out() << H("package "+pkg.name) << endl
      << endl;
  if (!pkg.annotation.empty())
    out() << pkg.annotation << endl
        << endl;
}

void Mdown::parameter(const doc::Parameter &p) {
  out() << BOLD(p.name()) << BR()
      << p.annotation() << endl
      << endl;
}

void Mdown::variable(const doc::Variable &var) {
  out() << HRULE()
      << H("variable "+var.name,3)
      << endl;
  if (!var.defaults.empty())
    out() << BOLD("Default:") << endl
        << endl
        << "    " << var.defaults << endl
        << endl;
  if (!var.annotation.empty())
    out() << var.annotation << endl
        << endl;
}

void Mdown::function(const doc::Function &func) {
  out() << HRULE()
      << H("function "+func.name,3)
      << endl
      << BOLD("Syntax:") << endl
      << endl
      // << CODE(signature(func)) << endl
      << "    " << func.signature() << endl
      << endl;
  if (!func.annotation.empty())
    out() << func.annotation << endl
        << endl;

  if (func.parameters.size()) {
    // how many annotated parameters do we have in place?
    auto annotations  = 0;
    for(auto i=func.parameters.begin();i!=func.parameters.end();++i) {
      annotations +=(!(*i)->annotation().empty());
    }
    if (annotations) {
      out() << BOLD("Parameters:") << endl
          << endl;
      for(auto i=func.parameters.begin();i!=func.parameters.end();i++) {
        auto p = **i;
        if (!p.annotation().empty())
          parameter(**i);
      }
      out() << endl;
    }
  }
}

void Mdown::module(const doc::Module &mod) {
  out() << HRULE()
      << H("module "+mod.name,3)
      << endl
      << BOLD("Syntax:") << endl
      << endl
      // << CODE(signature(mod)) << endl
      << "    " << mod.signature() << endl
      << endl;
  if (!mod.annotation.empty())
    out() << mod.annotation << endl
        << endl;

  if (mod.parameters.size()) {
    // how many annotated parameters do we have in place?
    auto annotations  = 0;
    for(auto i=mod.parameters.begin();i!=mod.parameters.end();++i) {
      annotations +=(!(*i)->annotation().empty());
    }
    if (annotations) {
      out() << BOLD("Parameters:") << endl
          << endl;
      for(auto i=mod.parameters.begin();i!=mod.parameters.end();i++) {
        auto p = **i;
        if (!p.annotation().empty())
          parameter(**i);
      }
      out() << endl;
    }
  }
}

void Mdown::operator () (const fs::path &source, const fs::path &droot, const doc::ItemMap &document) {
  assert(source.is_relative());
  assert(droot.is_absolute());

  cwd doc_root(droot);

  if (source.has_parent_path()) {
    doc::URI directory  = source.parent_path();
    if (!fs::exists(directory))
      fs::create_directory(directory);
  }

  auto md = source.parent_path() / source.stem().replace_extension(".md");
  _out = new ofstream(md);

  for (auto i=document.begin(); i!=document.end(); ++i) {
    auto pkg = i->second.get();
    if (is<Package>(*pkg)) {
      pkg->document = md;
      package(dynamic_cast<const Package&>(*pkg));
    }
  }

  if (size(document,typeid(Variable))) {
    out() << H("Variables",2) << endl 
        << endl;
    for (auto i=document.begin(); i!=document.end(); ++i) {
      auto var  = i->second.get();
      if (is<Variable>(*var)) {
        var->document = md;
        variable(dynamic_cast<const Variable&>(*var));
      }
    }
  }

  if (size(document,typeid(Function))) {
    out() << H("Functions",2) << endl 
        << endl;
    for (auto i=document.begin(); i!=document.end(); ++i) {
      auto func = i->second.get();
      if (is<Function>(*func)) {
        func->document = md;
        function(dynamic_cast<const Function&>(*func));
      }
    }
  }

  if (size(document,typeid(Module))) {
    out() << H("Modules",2) << endl 
        << endl;
    for (auto i=document.begin(); i!=document.end(); ++i) {
      auto mod = i->second.get();
      if (is<Module>(*mod)) {
        mod->document = md;
        module(dynamic_cast<const Module&>(*mod));
      }
    }
  }
  delete _out;
  _out = nullptr;
}

std::string Mdown::BOLD(const std::string &text) const {
  return "__"+text+"__";
}

std::string Mdown::BR() const {
  return "  \n";
}

std::string Mdown::H(const std::string &text,int level) const {
  string prefix;
  prefix.append(level,'#');
  return prefix + ' ' + text + '\n';
}

std::string Mdown::HRULE() const {
  return string("---\n\n");
}

std::string Mdown::CODE(const std::string &text) const {
  return "`"+text+"`";
}

void Mdown::operator () (const std::filesystem::path &droot, const Index &index) {
  assert(droot.is_absolute());

  cwd pwd(droot);
  _out = new ofstream("toc.md");

  index.serialize(*this);

  _out->close();
  delete _out;
  _out = nullptr;
}


}

}
