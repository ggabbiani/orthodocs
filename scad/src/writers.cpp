#include "globals.h"
#include "graph.h"
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

namespace markdown {

// transforms the passed string in a markdown id
void headingId(string &s) {
  replace( s.begin(), s.end(), ' ', '-'); // replace all ' ' to '-'
  s.erase(remove(s.begin(), s.end(), '/'), s.end());  // remove all '/'
}

}

void Mdown::package(ostream &out, const doc::Package &pkg) {
  out << H("package "+pkg.name) << endl
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

void Mdown::document(const fs::path &source, const Document &document) {
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

void Mdown::toc(const Index &index) {
  assert(option::droot.is_absolute());

  cwd pwd(option::droot);
  ofstream out("toc.md");
  out << H("Table of Contents",1) << endl;

  multimap<const string&,Item*> sub;
  char                          current = 0;
  for(auto &item: index._map) {
    // see https://www.markdownguide.org/extended-syntax/#heading-ids
    char  initial = std::toupper(item.second->indexKey()[0]);
    if (current!=initial) {  // new sub toc
      if (sub.size()) { // write previous sub toc
        subToc(sub,out,current);
        out << endl;
        sub.clear();
      }
      current = initial;
    }
    sub.emplace(item.first,item.second.get());
  }
  // write last sub toc
  if (sub.size()) { 
    subToc(sub,out,current);
  }
}

void Mdown::subToc(SubToc &sub,ostream &out,char &current) const {
  out << H(current,2) << endl;
  for(auto &i: sub) {
    auto id     = i.second->type()+'-'+i.second->name;
    markdown::headingId(id);
    auto link   = i.second->uri.string();
    auto title  = Index::title(*i.second);
    out << "- [" << title << "](" << link << "#" << id << ")" << endl;
  }
}

void Mdown::graph(ostream &out, const doc::Package &pkg) {
  IncLabel label("A");
  map<string,graph::Node> nodes;

  graph::Node src_node(&pkg,label);
  if (pkg.includes.size() || pkg.uses.size()) {

  }
}

void Mdown::graph(const Index &index) {
  cwd pwd(option::droot);
  ofstream out("deps.md");
  // ostream &out = cout;
  out << H("Dependencies",1) << '\n'
      << "```mermaid\n"
      << "graph TD" << endl;

  IncLabel label("A");
  map<string,graph::Node> nodes;

  for(auto &item: index.toc()) {
    doc::Package *src_package = dynamic_cast<doc::Package*>(item.second.get());
    if (src_package) {
      auto src_it = nodes.find(src_package->name);
      graph::Node src_node = src_it!=nodes.end() ? src_it->second : graph::Node(src_package,label);

      if (src_package->includes.size() || src_package->uses.size()) {
        for(auto &dst_name: src_package->includes) {
          auto dst_it = index._map.find(Package::indexKey(dst_name));
          doc::Package *dst_package = 
            dst_it!=index._map.end() ? dynamic_cast<doc::Package*>(dst_it->second.get()) : nullptr;
          if (dst_package) {
            auto dst_it = nodes.find(dst_package->name);
            graph::Node dst_node = dst_it!=nodes.end() ? dst_it->second : graph::Node(dst_package,label);

            graph::Connection connection(src_node,graph::Connection::Type::inc,dst_node);
            connection.write(out);
            if (src_it==nodes.end())
              nodes.emplace(src_package->name,src_node);
            if (dst_it==nodes.end())
              nodes.emplace(dst_package->name,dst_node);
          }
        }
        for(auto &dst_name: src_package->uses) {
          auto dst_it = index._map.find(Package::indexKey(dst_name));
          doc::Package *dst_package = 
            dst_it!=index._map.end() ? dynamic_cast<doc::Package*>(dst_it->second.get()) : nullptr;
          if (dst_package) {
            auto dst_it = nodes.find(dst_package->name);
            graph::Node dst_node = dst_it!=nodes.end() ? dst_it->second : graph::Node(dst_package,label);
            graph::Connection connection(src_node,graph::Connection::Type::use,dst_node);
            connection.write(out);
            if (src_it==nodes.end())
              nodes.emplace(src_package->name,src_node);
            if (dst_it==nodes.end())
              nodes.emplace(dst_package->name,dst_node);
          }
        }
      } else { // write an isolated node
        out << "    ";
        src_node.write(out);
        if (src_it==nodes.end())
          nodes.emplace(src_package->name,src_node);
        out << endl;
      }
    }
  }
  out  << "```" << endl;
}

}

}
