/*
 * language extension definition
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

#include "extension.h"
#include "listener.h"

#include <globals.h>
#include <utils.h>

#include "SCADLexer.h"
#include "SCADParser.h"

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
  Analysis::Results results;
  auto& data  = anno.data();
  for_each(slot.begin(),slot.end(),[&data,&results](const Slot &sl) {
    const char *t = data.c_str();
    cmatch match;
    while (regex_search(t, match, sl.regularExpression)) {
      auto offset = (t-data.c_str());
      xref::Analysis analysis {
        match.position(0)+offset,                              // position
        match.length(0),                                       // length
        data.substr(match.position(0)+offset,match.length(0)), // token to be substituted with reference
        data.substr(match.position(1)+offset,match.length(1))  // literal to be searched for in the exclusion vocabulary
      };
      // Analysis::Results::key_type uses the regex matching position, no
      // collision is possible, hence no need for checking try_emplace() result.
      results.try_emplace(analysis.position,analysis);
      t += analysis.position+analysis.length;
    }
  });
  set(anno,results);
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
  // source parse listener
  Listener  listener(source);
  // parse tree depth-first traverse
  tree::ParseTreeWalker  walker;
  // parsing
  tree::ParseTree       *tree = parser.pkg();

  // creation of the document
  walker.walk(&listener,tree);
  return listener.releaseDocument();
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
