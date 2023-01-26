/*
 * language extension definition
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

// private
#include "listener.h"
#include "visitor.h"

// public
#include <scad/extension.h>
#include <SCADLexer.h>
#include <SCADParser.h>

// components
#include <commons/config.h>
#include <commons/globals.h>
#include <commons/utils.h>

using namespace std;
using namespace antlr4;

namespace fs=std::filesystem;
namespace xref=::doc::xref;

namespace {

class ErrorHandler : public BaseErrorListener {
  void syntaxError(Recognizer *recognizer, Token * offendingSymbol, size_t line, size_t charPositionInLine,
    const string &msg, exception_ptr e) override;
};

void ErrorHandler::syntaxError(Recognizer *recognizer, Token * offendingSymbol, size_t line, size_t charPositionInLine, const std::string &msg, std::exception_ptr e) {
  ostringstream s;
  s << "Grammar(" << recognizer->getGrammarFileName() << ") Line(" << line << ":" << charPositionInLine << ") Error(" << msg << ')';
  throw std::invalid_argument(s.str());
}

ErrorHandler handler;

} // namespace

namespace scad {

const array<Extension::Slot,4> Extension::slot{{
  {"function",  [](const ::doc::Item *item) {return item->name+"()";},            regex("([a-zA-Z_][a-zA-Z0-9_]*)\\(\\)"    )},
  {"module",    [](const ::doc::Item *item) {return item->name+"{}";},            regex("([a-zA-Z_][a-zA-Z0-9_]*)\\{\\}"    )},
  {"package",   [](const ::doc::Item *item) {return item->type+' '+item->name;},  regex("package ([a-zA-Z_][a-zA-Z0-9_]*)"  )},
  {"variable",  [](const ::doc::Item *item) {return item->type+' '+item->name;},  regex("variable ([a-zA-Z_][a-zA-Z0-9_]*)" )}
}};

void Extension::analize(Annotation &anno) const {
  for_each(slot.begin(),slot.end(),[&anno,this](const Slot &sl) {
    auto &data  = anno.data();
    auto t      = data.c_str();
    cmatch match;
    while (regex_search(t, match, sl.regularExpression)) {
      auto offset   = (t-data.c_str());
      auto pos      = match.position(0)+offset;
      auto len      = (analitic::Data::Size)match.length(0);
      auto analysis = make_unique<xref::Analysis>(
        pos,                                                    // position
        len,                                                    // length
        data.substr(pos,match.length(0)),                       // token to be substituted with reference
        data.substr(match.position(1)+offset,match.length(1))   // literal to be searched for in the exclusion vocabulary
      );
      spdlog::trace("Adding analitic data token {}",analysis->token);
      add(anno,analysis.release());
      t += pos+len;
    }
  });
}

Extension::Extension() : language::Extension(ID) {
  ANTLRInputStream  in;
  SCADLexer         lexer(&in);
  auto              &vocabulary = lexer.getVocabulary();
  for(auto i=1;i<=vocabulary.getMaxTokenType();++i) {
    auto literal = vocabulary.getLiteralName(i);
    if (literal.size())
      _vocabulary.emplace(literal.substr(1,literal.length()-2));
  }
}

unique_ptr<::Document> Extension::parse(const fs::path &source) const {
  // change to source root
  cwd source_root(Option::sroot());
  ifstream          is(source);
  ANTLRInputStream  in(is);
  SCADLexer         lexer(&in);

  CommonTokenStream tokens(&lexer);
  SCADParser        parser(&tokens);

  // error management - FIXME: set once before this call
  parser.removeErrorListeners();
  parser.addErrorListener(&handler);
  // parsing
  auto tree = parser.pkg();

  #ifndef OPTION_SCAD_VISITORS

  // source parse listener
  Listener  listener(source,&tokens);
  // parse tree depth-first traverse
  tree::ParseTreeWalker  walker;
  // creation of the document
  walker.walk(&listener,tree);
  return listener.releaseDocument();

  #else // OPTION_SCAD_VISITORS

  Visitor visitor(source,&tokens);
  visitor.visitPkg(tree);
  return visitor.releaseDocument();

  #endif // OPTION_SCAD_VISITORS
}

const char *Extension::sourcePostfix() const {
  return ".scad";
}

language::Extension *Extension::builder(string_view language_id) {
  return (language_id==ID) ? &Singleton<Extension>::instance() : nullptr;
}

} // namespace scad

extern "C" {

language::Extension::Builder scad_builder = scad::Extension::builder;

}
