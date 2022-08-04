/*
 * ADOX scad processor
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

#include "antlr4-runtime.h"
#include "SCADLexer.h"
#include "SCADParser.h"
#include "SCADBaseListener.h"

#include <tree/ParseTreeWalker.h>

#include "CLI11.hpp"
#include <boost/filesystem.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace scad;
using namespace antlr4;
namespace fs=boost::filesystem;

class ErrorHandler : public BaseErrorListener {
  void syntaxError(Recognizer *recognizer, Token * offendingSymbol, size_t line, size_t charPositionInLine,
    const string &msg, exception_ptr e) override;
};

void ErrorHandler::syntaxError(Recognizer *recognizer, Token * offendingSymbol, size_t line, size_t charPositionInLine, const std::string &msg, std::exception_ptr e) {
  ostringstream s;
  // cout  << "Offending symbol  : " << offendingSymbol->toString() << endl
  //       << "Grammar file name : " << recognizer->getGrammarFileName() << endl;
  s << "Grammar(" << recognizer->getGrammarFileName() << ") Line(" << line << ":" << charPositionInLine << ") Error(" << msg << ")";
  throw std::invalid_argument(s.str());
}

class DocGenerator : public SCADBaseListener {
public:
  DocGenerator(const fs::path &source) : _package(source.filename().stem().string()) {}

  void enterPkg(SCADParser::PkgContext * ctx) override {
    cout << "Package " << _package << endl;
  }

  void enterNamed_function_definition(SCADParser::Named_function_definitionContext *ctx) override {
    auto identifier = ctx->ID()->getText();
    auto t = ctx->parameters_opt()->parameters();
    auto parms      = t ? t->getText() : string();

    cout << "Function " << identifier << "(" << parms <<  ")" << endl;
  }

  void enterNamed_module_definition(SCADParser::Named_module_definitionContext * ctx) override {
    auto identifier   = ctx->ID()->getText();
    auto parms        = ctx->parameters_opt()->parameters() ? ctx->parameters_opt()->parameters()->getText() : string("-");
    auto description  = "Not implemented yet.";
    ostringstream  ss;
    ss << identifier << "(" << parms <<  ")";
    auto signature    = ss.str();

    _index[identifier]  = description;
    cout << "Module " << signature << endl;
  }
private:
  // a little semantic...
  using ID = std::string;
  using Description = std::string;
  string _package;
  map<ID,Description> _index;
};

int main(int argc, const char *argv[]) {
  CLI::App app{"ADOX: automatic documentation generation for the OpenSCAD language."};
  auto      result = EXIT_SUCCESS;
  fs::path  file;
  bool      show_tokens = false;

  app.add_option("file", file, "SCAD source file")
    ->required()
    ->check(CLI::ExistingFile);
  app.add_flag("--tokens,-t",show_tokens,"Manages token listing");

  try {
    app.parse(argc, argv);
    ifstream          stream(file);
    ANTLRInputStream  input(stream);
    SCADLexer         lexer(&input);
    CommonTokenStream tokens(&lexer);

    if (show_tokens) {
      tokens.fill();
      for (auto token : tokens.getTokens())
        cout << token->toString() << endl;
    }

    SCADParser    parser(&tokens);

    // error management
    parser.removeErrorListeners();
    ErrorHandler listener;
    parser.addErrorListener(&listener);

    DocGenerator  generator(file.c_str());
    // parse tree depth-first traverse
    tree::ParseTreeWalker  walker;
    tree::ParseTree       *tree = parser.pkg();
    walker.walk(&generator,tree);
  } catch (const CLI::ParseError &e) {
    result  = app.exit(e);
  } catch(const exception &error) {
    cerr << error.what() << endl;
    result  = EXIT_FAILURE;
  }
  return result;
}
