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

// #define NDEBUG 

#include "processors.h"

#include "antlr4-runtime.h"
#include "CLI11.hpp"

#include <algorithm>

using namespace std;
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
  s << "Grammar(" << recognizer->getGrammarFileName() << ") Line(" << line << ":" << charPositionInLine << ") Error(" << msg << ')';
  throw std::invalid_argument(s.str());
}

//! return a vector containing all the file paths with «extension»
void lookup(
  //! list of source directories/files
  const vector<fs::path> &sources,
  //! extension to filter out
  const char *extension, 
  //! list of source files matching «extension»
  vector<fs::path> *result
) {
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
  assert(fs::is_directory(root));
  assert(fs::is_regular_file(file));
  auto aroot  = fs::absolute(root);
  auto afile  = fs::absolute(file);

  // auto      r_elem = aroot.begin();
  // auto      f_elem = afile.begin();
  fs::path  result;

  // while(r_elem!=aroot.end() && f_elem!=afile.end() && *r_elem==*f_elem) {
  //   ++r_elem;
  //   ++f_elem;
  // }

  // while(f_elem!=afile.end()) 
  //   result /= *f_elem++;

  auto elem = mismatch(aroot.begin(),aroot.end(),afile.begin(),afile.end()).second;
  while (elem!=afile.end())
    result /= *elem++;

  return result;
}

int main(int argc, const char *argv[]) {
  CLI::App app{"ADOX: automatic documentation generation for the OpenSCAD language."};
  auto              result = EXIT_SUCCESS;
  vector<fs::path>  sources;
  fs::path          sroot,droot;
  bool              show_tokens = false;

  app.add_option("sources", sources, "Source directories or files in any combination")
    ->required()
    // ->type_name("DIR|FILE")
    ->check(CLI::ExistingPath);
  app.add_option("-s,--src-root", sroot, "Source root")
    ->required()
    ->check(CLI::ExistingDirectory);
  app.add_option("-d,--doc-root",droot, "Document root")
    ->required()
    ->check(CLI::ExistingDirectory);
  app.add_flag("--tokens,-t",show_tokens,"Manages token listing");

  try {
    app.parse(argc, argv);
    vector<fs::path>  src_files;
    lookup(sources,".scad",&src_files);

    ErrorHandler    handler;
    scad::Processor proc(handler);
    for(auto file: src_files)
      proc(sroot,make_relative(sroot,file),droot);
  } catch (const CLI::ParseError &e) {
    result  = app.exit(e);
  } catch(const exception &error) {
    cerr << error.what() << endl;
    result  = EXIT_FAILURE;
  }
  return result;
}