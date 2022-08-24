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
#include "utils.h"

#include "antlr4-runtime.h"
#include "CLI11.hpp"

#include <boost/timer/progress_display.hpp>
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

int main(int argc, const char *argv[]) {
  CLI::App app{"ADOX: automatic documentation generation for the OpenSCAD language."};
  auto              result = EXIT_SUCCESS;
  vector<fs::path>  sources;
  fs::path          sroot,droot;
  bool              show_tokens = false;

  app.add_option("-s,--src-root", sroot, "Source root directory")
    ->required()
    ->check(CLI::ExistingDirectory);
  app.add_option("sources", sources, "Directories or files in any combination: paths can be passed either as relative to «Source root» or absolute")
    ->required()
    ->transform(CLI::Validator(
      [&sroot] (string &file) -> string {
        auto path = fs::path(file);
        if (path.is_relative())
          file = fs::path(sroot / path).string();
        return string();
      }
      ,"RELATIVE|ABSOLUTE"
    ))
    ->check(CLI::ExistingPath);
  app.add_option("-d,--doc-root",droot, "Document root directory")
    ->required()
    ->check(CLI::ExistingDirectory);

  try {
    app.parse(argc, argv);
    vector<fs::path>  src_files;
    lookup(sources,".scad",&src_files);

    ErrorHandler    handler;
    scad::Processor proc(handler);
    boost::timer::progress_display progress(src_files.size());
    for(auto file: src_files) {
      proc(sroot,fs::relative(file,sroot),droot);
      ++progress;
    }
  } catch (const CLI::ParseError &e) {
    result  = app.exit(e);
  } catch(const exception &error) {
    print_exception(error);
    result  = EXIT_FAILURE;
  }
  return result;
}
