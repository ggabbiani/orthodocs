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

#include "formatters.h"
#include "generator.h"

#include "antlr4-runtime.h"
#include "SCADLexer.h"
#include "SCADParser.h"

#include <tree/ParseTreeWalker.h>

#include "CLI11.hpp"
#include <filesystem>
#include <iostream>
#include <sstream>

using namespace std;
using namespace scad;
using namespace antlr4;

namespace fs=std::filesystem;

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

// return a vector containing all the file path with extension ".scad"
vector<fs::path> filter(const vector<fs::path> &sources,const char *extension) {
  vector<fs::path> files;
  for(auto &path: sources) {
    if (fs::is_regular_file(path)) {
      if (path.extension()==".scad")
        files.push_back(path);
    } else if (fs::is_directory(path)) {
      for (auto &entry: fs::directory_iterator{path}) 
        if (fs::is_regular_file(entry.path()) && entry.path().extension()==".scad")
          files.push_back(entry.path());
    }
  }
  return files;
}

int main(int argc, const char *argv[]) {
  CLI::App app{"ADOX: automatic documentation generation for the OpenSCAD language."};
  auto              result = EXIT_SUCCESS;
  vector<fs::path>  sources, src_files;
  fs::path          destination;
  bool              show_tokens = false;

  app.add_option("sources", sources, "Source directories or files in any combination")
    ->required()
    ->type_name("DIR|FILE");
  app.add_option("-d,--destination-directory", destination, "Destination directory")
    ->required()
    ->check(CLI::ExistingDirectory);
  app.add_flag("--tokens,-t",show_tokens,"Manages token listing");

  try {
    app.parse(argc, argv);
    src_files = filter(sources,".scad");

    cout  << "Source files:" << endl;
    for(auto file: src_files)
      cout << file << endl;

    // ifstream          is(file);
    // ANTLRInputStream  in(is);
    // SCADLexer         lexer(&in);
    // CommonTokenStream tokens(&lexer);

    // if (show_tokens) {
    //   tokens.fill();
    //   for (auto token : tokens.getTokens())
    //     cout << token->toString() << endl;
    // }

    // SCADParser    parser(&tokens);

    // // error management
    // parser.removeErrorListeners();
    // ErrorHandler listener;
    // parser.addErrorListener(&listener);

    // doc::Generator  generator(file.c_str());
    // // parse tree depth-first traverse
    // tree::ParseTreeWalker  walker;
    // tree::ParseTree       *tree = parser.pkg();
    // walker.walk(&generator,tree);

    // // documentation generation
    // ofstream os;
    // doc::formatter::Mdown markdown(os);
    // markdown.format(generator.items);

  } catch (const CLI::ParseError &e) {
    result  = app.exit(e);
  } catch(const exception &error) {
    cerr << error.what() << endl;
    result  = EXIT_FAILURE;
  }
  return result;
}
