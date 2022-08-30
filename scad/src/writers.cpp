#include "globals.h"
#include "xref.h"
#include "utils.h"
#include "writers.h"

#include <cassert>
#include <fstream>

using namespace std;
namespace fs  = std::filesystem;

namespace doc {

size_t size(const Document &items,const type_info &type) {
  auto size  = 0;
  for(auto i=items.begin(); i!=items.end(); ++i) {
    auto var  = i->second.get();
    size += (typeid(*var)==type);
  }
  return size;
}

namespace writer {

void Mdown::package(ostream &out, const doc::Package &pkg) {
  out << H("package "+(pkg.path.parent_path()/pkg.path.stem()).string()) << endl
      << endl;
  if (!pkg.includes.empty()) {
    out << BOLD("Includes:") << '\n'
        << endl;
    for(auto p: pkg.includes) {
      out << "    " << p << '\n';
    }
    out << endl;
  }
  if (!pkg.uses.empty()) {
    out << BOLD("Uses:") << endl
        << endl;
    for(auto p: pkg.uses) {
      out << "    " << p << endl
          << endl;
    }
  }
  if (!pkg.annotation.empty())
    out << pkg.annotation << endl
        << endl;
}

void Mdown::parameter(ostream &out, const doc::Parameter &p) {
  out << BOLD(p.name()) << BR()
      << p.annotation() << endl
      << endl;
}

void Mdown::variable(ostream &out, const doc::Variable &var) {
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

void Mdown::function(ostream &out, const doc::Function &func) {
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
          parameter(out,**i);
      }
      out << endl;
    }
  }
}

void Mdown::module(ostream &out, const doc::Module &mod) {
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
          parameter(out,**i);
      }
      out << endl;
    }
  }
}

void Mdown::operator () (const fs::path &source, const Document &document) {
  assert(source.is_relative());
  assert(option::droot.is_absolute());

  cwd doc_root(option::droot);

  if (source.has_parent_path()) {
    doc::URI directory  = source.parent_path();
    if (!fs::exists(directory))
      fs::create_directory(directory);
  }

  auto md = source.parent_path() / source.stem().replace_extension(".md");
  ofstream out(md);

  for (auto i=document.begin(); i!=document.end(); ++i) {
    auto pkg = i->second.get();
    if (is<Package>(*pkg)) {
      pkg->uri = md;
      package(out,dynamic_cast<const Package&>(*pkg));
    }
  }

  if (size(document,typeid(Variable))) {
    out << H("Variables",2) << endl 
        << endl;
    for (auto i=document.begin(); i!=document.end(); ++i) {
      auto var  = i->second.get();
      if (is<Variable>(*var)) {
        var->uri = md;
        variable(out,dynamic_cast<const Variable&>(*var));
      }
    }
  }

  if (size(document,typeid(Function))) {
    out << H("Functions",2) << endl 
        << endl;
    for (auto i=document.begin(); i!=document.end(); ++i) {
      auto func = i->second.get();
      if (is<Function>(*func)) {
        func->uri = md;
        function(out,dynamic_cast<const Function&>(*func));
      }
    }
  }

  if (size(document,typeid(Module))) {
    out << H("Modules",2) << endl 
        << endl;
    for (auto i=document.begin(); i!=document.end(); ++i) {
      auto mod = i->second.get();
      if (is<Module>(*mod)) {
        mod->uri = md;
        module(out,dynamic_cast<const Module&>(*mod));
      }
    }
  }
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

std::string Mdown::H(char c,int level) const {
  string prefix;
  prefix.append(level,'#');
  return prefix+' '+c+'\n';
}

std::string Mdown::HRULE() const {
  return string("---\n\n");
}

std::string Mdown::CODE(const std::string &text) const {
  return "`"+text+"`";
}

void Mdown::operator () (const Index &index) {
  assert(option::droot.is_absolute());

  cwd pwd(option::droot);
  ofstream out("toc.md");
  out << H("Table of Contents",1) << endl;

  multimap<const string&,Item*> sub;
  char                          current = 0;
  for(auto &item: index._map) {
    // see https://www.markdownguide.org/extended-syntax/#heading-ids
    char  initial = std::toupper(Index::key(*item.second)[0]);
    if (current!=initial) {  // new sub toc
      if (sub.size()) { // write previous sub toc
        out << H(current,2) << '\n';
        for(auto &i: sub) {
          auto id     = i.second->type()+'-'+i.second->name;
          auto link   = i.second->uri.string();
          auto title  = Index::title(*i.second);
          out << "- [" << title << "](" << link << "#" << id << ")\n";
        }
        out << endl;
        sub.clear();
      }
      current = initial;
    }
    sub.emplace(item.first,item.second.get());
  }
  // write last sub toc
  if (sub.size()) { 
    out << H(current,2) << endl;
    for(auto &i: sub) {
      auto id     = i.second->type()+'-'+i.second->name;
      auto link   = i.second->uri.string();
      auto title  = Index::title(*i.second);
      out << "- [" << title << "](" << link << "#" << id << ")" << endl;
    }
  }
}

}

}
