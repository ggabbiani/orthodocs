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
void lookup(const vector<fs::path> &sources,const char *extension,vector<fs::path> *result) {
  for(auto &path: sources) {
    if (fs::is_regular_file(path)) {
      if (path.extension()==".scad")
        result->push_back(path);
    } else if (fs::is_directory(path)) {
      for (auto &entry: fs::directory_iterator{path}) {
        auto path = entry.path();
        if (fs::is_regular_file(path)) {
          if (path.extension()==".scad")
            result->push_back(path);
        } else if (fs::is_directory(path)) {
          lookup(vector<fs::path>{path},extension,result);
        }
      }
    }
  }
}

fs::path make_relative(fs::path root, fs::path file) {
  auto aroot  = fs::absolute(root.remove_filename());
  auto afile  = fs::absolute(file);
  auto f      = afile.begin();
  cout << "afile: " << afile << endl
    << "aroot: " << aroot << endl;
  for(auto r=aroot.begin();r!=aroot.end()&&f!=afile.end();++r,++f) {
    if (*r!=*f) {
      stringstream ss;
      ss << *r << "!=" << *f;
      throw runtime_error(ss.str());
    }
    cout << "r: " << *r << endl
      << (*r==*f) << endl
      << (r!=aroot.end()) << endl;
  }
  cout << "f: " << *f << endl;
  return accumulate(
    next(--f), 
    afile.end(), 
    fs::path{}, 
    divides{}
  );
}

int main(int argc, const char *argv[]) {
  CLI::App app{"ADOX: automatic documentation generation for the OpenSCAD language."};
  auto              result = EXIT_SUCCESS;
  vector<fs::path>  sources, src_files;
  fs::path          sroot,droot;
  bool              show_tokens = false;

  app.add_option("sources", sources, "Source directories or files in any combination")
    ->required()
    // ->type_name("DIR|FILE")
    ->check(CLI::ExistingPath);
  app.add_option("-s,--source-root", sroot, "Source directory root")
    ->check(CLI::ExistingDirectory);
  app.add_option("-d,--doc-root",droot, "Document root")
    ->required()
    ->check(CLI::ExistingDirectory);
  app.add_flag("--tokens,-t",show_tokens,"Manages token listing");

  try {
    app.parse(argc, argv);
    lookup(sources,".scad",&src_files);

    cout  << "Source files:" << endl;
    for(auto file: src_files)
      cout 
        // << file << endl
        << make_relative(sroot,file) << endl;

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
