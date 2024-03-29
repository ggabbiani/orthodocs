/*
 * markdown extension definition
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "extension.h"
#include "graph.h"

#include <commons/bar.h>
#include <commons/spdx.h>

#include <spdlog/spdlog.h>
#include <boost/algorithm/string/predicate.hpp>

#include <cassert>
#include <fstream>

using namespace std;
namespace fs=std::filesystem;
namespace xref=::doc::xref;

namespace {

/*!
 * returns the passed string as a markdown id.
 *
 * Implementation of [GitLab Flavored Markdown (GLFM) | GitLab](https://docs.gitlab.com/ee/user/markdown.html#header-ids-and-links)
 *
 * The IDs generated should be conformant to the following rules:
 *
 * 1. All text is converted to lowercase.
 * 2. All non-word text (such as punctuation or HTML) is removed.
 * 3. All spaces are converted to hyphens.
 * 4. Two or more hyphens in a row are converted to one.
 * 5. If a header with the same ID has already been generated, a unique incrementing number is appended, starting at 1.
 *
 * NOTE: in our case rule 5 should never occur
 */
string headingId(string_view s) {
  string result;
  result.reserve(s.size());
  string::value_type prev=0;
  for_each(s.begin(),s.end(),
    [&prev,&result] (string::value_type c) {
      if (c=='_'||c=='$')   // rule 1 shortcut for legal chars with no lowercase
        prev  = c;
      else if (isalnum(c))  // rule 1
        prev  = static_cast<string::value_type>(tolower(c));
      else if ((isblank(c)||c=='-') && prev!='-') // rule 3 and 4
        prev  = '-';
      else                  // rule 2
        return;
      result.push_back(prev);
    }
  );
  return result;
}

using Filter = function<bool(const graph::Connection&)>;

/**
 * save to «out» the «src_package» dependencies in mermaid format.
 * NOTE: must be preceeded and followed by proper mermaid open/close statements
 */
void saveme(
  const scad::doc::Package  *src_package,
  graph::Node::Map          &nodemap,
  IncLabel                  &label,
  ostream                   &out,
  const Filter              &filter= [] (const graph::Connection &) {return true;}
) {
  auto src_it   = nodemap.find(src_package->name);
  auto src_node = src_it!=nodemap.end() ? src_it->second : graph::Node(src_package->path,label);

  auto written = count_if(
    src_package->includes.begin(),
    src_package->includes.end(),
    [&out,&filter,&src_node,&label,&nodemap] (const decltype(src_package->includes)::value_type &dst_name) {
      auto dst_it   = nodemap.find(dst_name);
      auto dst_node = dst_it!=nodemap.end() ? dst_it->second : graph::Node(dst_name,label);
      if (graph::Connection connection(src_node,graph::Connection::Type::inc,dst_node); filter(connection)) {
        connection.write(out,nodemap);
        return true;
      }
      return false;
    }
  );

  written += count_if(
    src_package->uses.begin(),
    src_package->uses.end(),
    [&out,&filter,&src_node,&label,&nodemap] (const decltype(src_package->uses)::value_type &dst_name) {
      auto dst_it   = nodemap.find(dst_name);
      auto dst_node = dst_it!=nodemap.end() ? dst_it->second : graph::Node(dst_name,label);
      if (graph::Connection connection(src_node,graph::Connection::Type::use,dst_node); filter(connection)) {
        connection.write(out,nodemap);
        return true;
      }
      return false;
    }
  );

  if (!written) { // write an isolated node
    out << "    ";
    src_node.write(out,nodemap);
    out << endl;
  }
}

}

namespace markdown {

Extension::Extension(Dictionary &dict,const language::Extension *lang)
  : writer::Extension(ID,dict,lang->vocabulary(),lang) {
}

void Extension::save(const Document &doc) {
  auto &source = doc.source;
  assert(source.is_relative());
  assert(cli::docRoot().is_absolute());

  try {
    cwd doc_root(cli::docRoot());

    if (source.has_parent_path()) {
      ::doc::URI directory  = source.parent_path();
      if (!fs::exists(directory))
        fs::create_directory(directory);
    }

    auto md = source.parent_path() / source.stem().replace_extension(".md");
    ofstream out(md);

    write(doc,(Package*)Document::Header<Package>(doc),out);
    write(doc,Document::Topic<Variable> (doc,"Variables"),out);
    write(doc,Document::Topic<Function> (doc,"Functions"),out);
    write(doc,Document::Topic<Module>   (doc,"Modules"),out);
  } catch(...) {
    throw_with_nested(runtime_error(ERR_CALL(doc.source)));
  }
}

void Extension::save(const ToC &toc) {
  assert(cli::docRoot().is_absolute());

  try {
    ::Bar bar(toc,"items in ToC");
    cwd pwd(cli::docRoot());
    fs::path document_source("toc.md");
    ofstream out(document_source);
    out << H("Table of Contents",1) << endl;
    ::doc::SubToC sub;
    char current = 0;
    for(auto item: toc) {
      bar.status(item->tocKey());
      // see https://www.markdownguide.org/extended-syntax/#heading-ids
      if (auto initial=(char)std::toupper(item->tocKey()[0]); current!=initial) {  // new sub toc
        if (sub.size()) { // write previous sub toc
          subToc(document_source,sub,out,current);
          out << endl;
          sub.clear();
        }
        current = initial;
      }
      sub.emplace(item);
      bar++;
    }
    // write last sub toc
    if (sub.size()) {
      subToc(document_source,sub,out,current);
    }
  } catch(...) {
    indicators::show_console_cursor(true);
    throw;
  }

}

void Extension::subToc(const std::filesystem::path &document_source, const SubToC &sub,ostream &out,char current) const {
  out << H(current,2) << endl;
  for(const auto *item: sub) {
    auto title  = ::doc::toc::title(*item);
    out << "- [" << title << "](" << reference(item,&document_source) << ")" << endl;
  }
}

void Extension::write(const Document &document, const Package *pkg,ostream &out) const {
  assert(pkg);
  out << H("package "+pkg->name)
      << endl;

  if (pkg->includes.size() || pkg->uses.size()) {
    if (boost::iequals(cli::pkg_deps(),"graph")) {
      out << H("Dependencies",2) << '\n'
          << "```mermaid\n"
          << "graph LR" << endl;
      graph(*pkg,out);
      out  << "```\n" << endl;
    } else {
      if (!pkg->includes.empty()) {
        out << BOLD("Includes:") << '\n'
            << endl;
        for(const auto &p: pkg->includes)
          out << "    " << p << '\n';
        out << endl;
      }
      if (!pkg->uses.empty()) {
        out << BOLD("Uses:") << '\n'
            << '\n';
        for(const auto &p: pkg->uses)
          out << "    " << p << '\n';
        out << endl;
      }
    }
  }
  // write annotation contents
  write(document,pkg->annotation,out);
}

void Extension::write(const Document &document, const Annotation &annotation, ostream &out) const {
  try {
    if (!annotation.empty()) {
      string s = annotation.data();
      auto &results = annotation.analytics();
      // xref substitution starts from last occurrence
      for_each(results.rbegin(), results.rend(),
        [this,&document,&s] (const Analytics::value_type &value) {
          // check if the analytic result is an xref result
          if (auto xref_data = dynamic_cast<const xref::Analysis*>(value.second.get()); xref_data) {
            if (auto i=this->_dict->find(xref_data->token); i!=this->_dict->end()) {
              auto ref    = this->reference(i->second,&document.source);
              string link = "["+xref_data->token+"]("+ref+")";
              s.replace(xref_data->position,xref_data->length,link);
            } else if (const auto &l=_vocabulary.find(xref_data->literal);l==_vocabulary.end()) {
              spdlog::warn("Item '{}' in document '{}' not present in the inclusion dictionary nor in the exclusion vocabulary", xref_data->token, document.source.string());
            }
          } else if (auto lic_data = dynamic_cast<const spdx::Data*>(value.second.get()); lic_data) {
            auto link32 = U"["+utf32(s).substr(lic_data->position,lic_data->length)+U"]("+utf32(lic_data->url)+U")";
            auto s32    = utf32(s);
            s           = utf8(s32.replace(lic_data->position,lic_data->length,link32));
          }
        }
      );
      out << s << '\n' << endl;
    }
  } catch(...) {
    throw_with_nested(runtime_error(ERR_CALL()));
  }
}

void Extension::write(const Document &document, const Parameter *param, ostream &out) const {
  out << BOLD(param->name) << BR();
  write(document,param->annotation,out);
}

void Extension::write(const Document &document, const Variable *var, ostream &out) const {
  assert(var);
  out << HRULE()
      << H("variable "+var->name,3)
      << endl;
  if (!var->defaults.empty())
    out << BOLD("Default:") << endl
        << endl
        << "    " << var->defaults << endl
        << endl;
  write(document, var->annotation,out);
}

void Extension::write(const Document &document, const Function *func, ostream &out) const {
  assert(func);
  out << HRULE()
      << H("function "+func->name,3)
      << endl
      << BOLD("Syntax:") << endl
      << endl
      << "```text\n"
      << func->signature() << '\n'
      << "```\n"
      << endl;
  write(document,func->annotation,out);

  if (func->parameters.size()) {
    // how many annotated parameters do we have in place?
    auto annotations = count_if(
      func->parameters.begin(),
      func->parameters.end(),
      [] (const decltype(func->parameters)::value_type &param) {
        return !param->annotation.empty();
      }
    );

    if (annotations) {
      out << BOLD("Parameters:") << endl
          << endl;
      for_each(
        func->parameters.begin(),
        func->parameters.end(),
        [this,&document,&out] (const decltype(func->parameters)::value_type &param) {
          if (!param->annotation.empty())
            write(document,param.get(),out);
        }
      );
      out << endl;
    }
  }
}

void Extension::write(const Document &document, const Module *mod, ostream &out) const {
  assert(mod);
  out << HRULE()
      << H("module "+mod->name,3)
      << endl
      << BOLD("Syntax:") << endl
      << endl
      // << CODE(signature(mod)) << endl
      << "    " << mod->signature() << endl
      << endl;
  write(document,mod->annotation,out);

  if (mod->parameters.size()) {
    // how many annotated parameters do we have in place?
    auto annotations = count_if(mod->parameters.begin(),mod->parameters.end(),
      [] (const ::doc::Parameter::Owner &p) {
        return !p->annotation.empty();
      }
    );
    if (annotations) {
      out << BOLD("Parameters:") << endl
          << endl;
      for_each(mod->parameters.begin(),mod->parameters.end(),
        [this,&document,&out] (const ::doc::Parameter::Owner &param) {
          if (!param->annotation.empty())
            write(document,param.get(),out);
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

void Extension::graph(const Package &pkg, ostream &out) const {
  IncLabel label("A");
  graph::Node::Map nodemap;
  saveme(&pkg,nodemap,label,out);
}

void Extension::graphs(const ToC &toc, const FileSet &dirs) {
  try {
    ::Bar bar(dirs,"graphs created");
    // change working directory to «document root»
    cwd document_root(cli::docRoot());
    // from here we move on each directory passed in the FileSet «dirs»
    for(auto &dir: dirs) {
      bar.status(dir.string());
      assert(dir.is_relative());
      // select only doc::Package items
      auto packages = ::doc::toc::filter(dir,toc,[dir] (const fs::path &,const ::doc::Item *item) {
        if (auto package = dynamic_cast<const Package*>(item); package)
          return dir=="." ? true : is_sub_of(package->path.parent_path(),dir);
        else
          return false;
      });
      // change the working directory to «dir»
      cwd graph_dir(dir);
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
      for(const auto *item: packages) {
        auto package = dynamic_cast<const Package*>(item);
        assert(package);
        if (dir!=".") {
          if (is_sub_of(package->path.parent_path(),dir))
            saveme(package,nodemap,label,out, [dir] (const graph::Connection &conn) {
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

string Extension::reference(const Item *item, const fs::path *document_source) const {
  auto id       = headingId(item->type+'-'+item->name);
  auto package  = dynamic_cast<const Package*>(item);
  if (!package)
    package = dynamic_cast<const Package*>(item->parent);
  assert(package);
  auto same_document = document_source ? package->path.parent_path()==document_source->parent_path() && package->path.stem() == document_source->stem() : true;
  if (same_document)
    return "#"+id;
  else {
    auto base = document_source->parent_path();
    auto link = base.empty() ? package->path : fs::relative(package->path,base);
    return (link.parent_path()/package->path.stem()).string()+".md#"+id;
  }
}

writer::Extension *Extension::builder(string_view writer_id,Dictionary &dict,const language::Extension *lang) {
  return (writer_id==ID) ? &Singleton<Extension>::instance(dict,lang) : nullptr;
}

} // namespace markdown

extern "C" {

writer::Extension::Builder markdown_builder = markdown::Extension::builder;

}
