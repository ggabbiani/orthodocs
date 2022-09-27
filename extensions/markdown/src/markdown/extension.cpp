/*
 * markdown extention definition
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

#include "orthodocs/bar.h"
#include "markdown/extension.h"
#include "markdown/graph.h"

#include <boost/algorithm/string.hpp>

#include <cassert>
#include <fstream>

using namespace std;
namespace fs=std::filesystem;

ExtensionLoader<markdown::Extension> markdown_extension_loader;

namespace {

// transforms the passed string in a markdown id
void headingId(string &s) {
  replace( s.begin(), s.end(), ' ', '-'); // replace all ' ' to '-'
  s.erase(remove(s.begin(), s.end(), '/'), s.end());  // remove all '/'
}

/**
 * save to «out» the «src_package» dependencies in mermaid format.
 * NOTE: must be preceeded and followed by proper mermaid open/close statements
 */
void saveme(const scad::doc::Package *src_package, 
  graph::Node::Map &nodemap, 
  IncLabel &label, 
  ostream &out,
  std::function<bool(const graph::Connection &)> filter= [](const graph::Connection &) {return true;}
) {
  auto src_it = nodemap.find(src_package->name);
  graph::Node src_node = src_it!=nodemap.end() ? src_it->second : graph::Node(src_package->path,label);
  auto written = 0;
  for(auto &dst_name: src_package->includes) {
    auto dst_it = nodemap.find(dst_name);
    graph::Node dst_node = dst_it!=nodemap.end() ? dst_it->second : graph::Node(dst_name,label);
    graph::Connection connection(src_node,graph::Connection::Type::inc,dst_node);
    if (filter(connection)) {
      connection.write(out,nodemap);
      ++written;
    }
  }
  for(auto &dst_name: src_package->uses) {
    auto dst_it = nodemap.find(dst_name);
    graph::Node dst_node = dst_it!=nodemap.end() ? dst_it->second : graph::Node(dst_name,label);
    graph::Connection connection(src_node,graph::Connection::Type::use,dst_node);
    if (filter(connection)) {
      connection.write(out,nodemap);
      ++written;
    }
  }
  if (!written) { // write an isolated node
    out << "    ";
    src_node.write(out,nodemap);
    out << endl;
  }
}

}

namespace markdown {

void Extension::save(const orthodocs::Document &document) {
  auto &source = document.source;
  assert(source.is_relative());
  assert(Option::droot().is_absolute());

  cwd doc_root(Option::droot());

  if (source.has_parent_path()) {
    orthodocs::doc::URI directory  = source.parent_path();
    if (!fs::exists(directory))
      fs::create_directory(directory);
  }

  auto md = source.parent_path() / source.stem().replace_extension(".md");
  ofstream out(md);

  for (auto i=document.begin(); i!=document.end(); ++i) {
    auto pkg = i->second.get();
    if (is<scad::doc::Package>(*pkg)) {
      pkg->uri = md;
      package(out,dynamic_cast<const scad::doc::Package&>(*pkg));
    }
  }

  if (document.size<scad::doc::Variable>()) {
    out << H("Variables",2)
        << endl;
    for (auto i=document.begin(); i!=document.end(); ++i) {
      auto var  = i->second.get();
      if (is<scad::doc::Variable>(*var)) {
        var->uri = md;
        variable(out,dynamic_cast<const scad::doc::Variable&>(*var));
      }
    }
  }

  if (document.size<scad::doc::Function>()) {
    out << H("Functions",2)
        << endl;
    for (auto i=document.begin(); i!=document.end(); ++i) {
      auto func = i->second.get();
      if (is<scad::doc::Function>(*func)) {
        func->uri = md;
        function(out,dynamic_cast<const scad::doc::Function&>(*func));
      }
    }
  }

  if (document.size<scad::doc::Module>()) {
    out << H("Modules",2) << endl 
        << endl;
    for (auto i=document.begin(); i!=document.end(); ++i) {
      auto mod = i->second.get();
      if (is<scad::doc::Module>(*mod)) {
        mod->uri = md;
        module(out,dynamic_cast<const scad::doc::Module&>(*mod));
      }
    }
  }
}

void Extension::save(const orthodocs::doc::ToC &toc) {
  assert(Option::droot().is_absolute());

  try {
    orthodocs::Bar bar(toc,"items in ToC");
    cwd pwd(Option::droot());
    ofstream out("toc.md");
    out << H("Table of Contents",1) << endl;
    orthodocs::doc::SubToC sub;
    char current = 0;
    for(auto &[key, item]: toc) {
      bar.status(key);
      // see https://www.markdownguide.org/extended-syntax/#heading-ids
      if (auto initial=(char)std::toupper(item->indexKey()[0]); current!=initial) {  // new sub toc
        if (sub.size()) { // write previous sub toc
          subToc(sub,out,current);
          out << endl;
          sub.clear();
        }
        current = initial;
      }
      sub.emplace(key,item);
      bar++;
    }
    // write last sub toc
    if (sub.size()) { 
      subToc(sub,out,current);
    }
  } catch(...) {
    indicators::show_console_cursor(true);
    throw;
  }

}

void Extension::subToc(const orthodocs::doc::SubToC &sub,ostream &out,char current) const {
  out << H(current,2) << endl;
  for(auto &[key, item]: sub) {
    auto id     = item->type()+'-'+item->name;
    headingId(id);
    auto link   = item->uri.string();
    auto title  = orthodocs::doc::toc::title(*item);
    out << "- [" << title << "](" << link << "#" << id << ")" << endl;
  }
}

void Extension::package(ostream &out, const scad::doc::Package &pkg) {
  out << H("package "+pkg.name)
      << endl;

  if (pkg.includes.size() || pkg.uses.size()) {
    if (boost::iequals(Option::pkg_deps(),"graph")) {
      out << H("Dependencies",2) << '\n'
          << "```mermaid\n"
          << "graph LR" << endl;
      graph(pkg,out);
      out  << "```\n" << endl;
    } else {
      if (!pkg.includes.empty()) {
        out << BOLD("Includes:") << '\n'
            << endl;
        for(const auto &p: pkg.includes)
          out << "    " << p << '\n';
        out << endl;
      }
      if (!pkg.uses.empty()) {
        out << BOLD("Uses:") << '\n'
            << '\n';
        for(const auto &p: pkg.uses) 
          out << "    " << p << '\n';
        out << endl;
      }
    }
  }
  // write annotation contents
  if (!pkg.annotation.empty()) {
    out << pkg.annotation << endl;
    if (pkg.license) 
      out << "*Published under " << "__" << pkg.license << "__*" << '\n' << endl;
  }
}

void Extension::parameter(ostream &out, const orthodocs::doc::Parameter &p) const {
  out << BOLD(p.name()) << BR()
      << p.annotation() << endl
      << endl;
}

void Extension::variable(ostream &out, const scad::doc::Variable &var) const {
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

void Extension::function(ostream &out, const scad::doc::Function &func) const {
  out << HRULE()
      << H("function "+func.name,3)
      << endl
      << BOLD("Syntax:") << endl
      << endl
      // << CODE(signature(func)) << endl
      << "```text\n"
      << func.signature() << '\n'
      << "```\n"
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

void Extension::module(ostream &out, const scad::doc::Module &mod) const {
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
    auto annotations = count_if(mod.parameters.begin(),mod.parameters.end(),
      [] (const orthodocs::doc::ParameterPtr &p) {
        return !p->annotation().empty();
      }
    );
    if (annotations) {
      out << BOLD("Parameters:") << endl
          << endl;
      for_each(mod.parameters.begin(),mod.parameters.end(),
        [this,&out] (const orthodocs::doc::ParameterPtr &p) {
          if (!p->annotation().empty())
            parameter(out,*p);
        }
      );
      out << endl;
    }
  }
}

std::string Extension::BOLD(const std::string &text) const {
  return "__"+text+"__";
}

std::string Extension::ITALIC(const std::string &text) const {
  return "*"+text+"*";
}

std::string Extension::BR() const {
  return "  \n";
}

std::string Extension::H(const std::string &text,int level) const {
  string prefix;
  prefix.append(level,'#');
  return prefix + ' ' + text + '\n';
}

std::string Extension::H(char c,int level) const {
  string prefix;
  prefix.append(level,'#');
  return prefix+' '+c+'\n';
}

std::string Extension::HRULE() const {
  return string("---\n\n");
}

std::string Extension::CODE(const std::string &text) const {
  return "`"+text+"`";
}

void Extension::graph(const scad::doc::Package &pkg, ostream &out) {
  IncLabel label("A");
  graph::Node::Map nodemap;
  saveme(&pkg,nodemap,label,out);
}

void Extension::graphs(const orthodocs::doc::ToC &toc, const FileSet &dirs) {
  try {
    orthodocs::Bar bar(dirs,"graphs created");
    // change working directory to «document root»
    cwd pwd(Option::droot());
    // from here we move on each directory passed in the FileSet
    for(auto &dir: dirs) {
      bar.status(dir.string());
      // cout << "dir: " << dir << endl;
      assert(dir.is_relative());
      // select only doc::Package items
      auto packages = orthodocs::doc::toc::filter(dir,toc,[dir] (const fs::path &path,const orthodocs::doc::Item *item) -> bool {
        bool result;
        auto package = dynamic_cast<const scad::doc::Package*>(item);
        if (package) {
          result = dir=="." ? true : is_sub_of(package->path.parent_path(),dir);
        } else
          result = false;
        return (result);
      });
      // change the working directory to «dir»
      cwd pwd(dir);
      // create the sub graph file
      ofstream out("deps.md");
      // allocate graph resources on stack
      graph::Node::Map nodemap;
      IncLabel label("A");
      // define a top-down mermaid graph
      out << H("Dependencies",1) << '\n'
          << "```mermaid\n"
          << "graph TD" << endl;
      // build a dependency graph limited on packages located inside «current_dir»
      for(auto element: packages) {
        const scad::doc::Package *package = dynamic_cast<const scad::doc::Package*>(element.second);
        assert(package);
        // FIXME: unclear behaviour of path when using "." (current directory)
        if (dir!=".") {
          if (is_sub_of(package->path.parent_path(),dir))
            saveme(package,nodemap,label,out, [dir] (const graph::Connection &conn) -> bool {
              return (is_sub_of(conn.destination.path.parent_path(),dir));
          });
        } else 
          saveme(package,nodemap,label,out);
      }
      out  << "```\n" << endl;
      bar++;
    }
  } catch(...) {
    indicators::show_console_cursor(true);
    throw;
  }
}

} // namespace markdown
