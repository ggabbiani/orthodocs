/*
 * OpenSCAD source file processor
 *
 * Copyright © 2022 Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * This file is part of the 'AutoDox' (ADOX) project.
 *
 * ADOX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ADOX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ADOX.  If not, see <http: //www.gnu.org/licenses/>.
 */

#include "generator.h"
#include "globals.h"
#include "processors.h"
#include "utils.h"
#include "xref.h"
#include "writers.h"

#include "antlr4-runtime.h"
#include "SCADLexer.h"
#include "SCADParser.h"
#include "tree/ParseTreeWalker.h"

using namespace std;
using namespace antlr4;

namespace fs = std::filesystem;

class ErrorHandler : public BaseErrorListener {
  void syntaxError(Recognizer *recognizer, Token * offendingSymbol, size_t line, size_t charPositionInLine,
    const string &msg, exception_ptr e) override;
};

void ErrorHandler::syntaxError(Recognizer *recognizer, Token * offendingSymbol, size_t line, size_t charPositionInLine, const std::string &msg, std::exception_ptr e) {
  ostringstream s;
  // cout  << "Offending symbol  : " << offendingSymbol->toString() << endl
  //       << "Grammar file name : " << recognizer->getGrammarFileName() << endl;
  s << "Grammar(" << recognizer->getGrammarFileName() << ") Line(" << line << ":" << charPositionInLine << ") Error(" << msg << ')';
  throw std::invalid_argument(s.str());
}

static ErrorHandler handler;

namespace scad {

void Processor::operator () (const fs::path &source) {
  assert(source.is_relative());
  assert(source.has_filename());
  try {
    // change to source root
    cwd source_root(_sroot);

    ifstream          is(source);
    ANTLRInputStream  in(is);
    SCADLexer         lexer(&in);
    CommonTokenStream tokens(&lexer);
    SCADParser        parser(&tokens);

    // error management
    parser.removeErrorListeners();
    parser.addErrorListener(&handler);

    // source parse listener
    Generator  listener(source.filename().stem().c_str());
    // parse tree depth-first traverse
    tree::ParseTreeWalker  walker;
    // parsing
    tree::ParseTree       *tree = parser.pkg();
    // creation of the document
    walker.walk(&listener,tree);

    // document writing

    _writer->operator()(source,_droot,listener.document);
    _toc.add(listener.document);

  } catch(...) {
    throw_with_nested(runtime_error("error while processing '"+source.string()+'\''));
  }
}

}
