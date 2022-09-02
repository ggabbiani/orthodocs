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

#include "globals.h"
#include "processors.h"
#include "utils.h"
#include "antlr4-runtime.h"

#include "indicators/block_progress_bar.hpp"
#include "indicators/cursor_control.hpp"
#include "CLI11.hpp"

#include <algorithm>

using namespace std;
using namespace antlr4;

namespace fs=std::filesystem;

string cannonau(string &file) {
  auto path = fs::path(file);
  if (path.is_relative()) {
    path  = fs::canonical(path);
    file  = path.string();
  }
  return fs::is_directory(path) ? string() : string("Root directory does not exist : ")+path.string();
}

int main(int argc, const char *argv[]) {
  CLI::App  app{"Automatic documentation generation for the OpenSCAD language.","adox-scad"};
  auto      result = EXIT_SUCCESS;
  FileSet   sources;

  app.add_flag("-a,--admonitions",option::admonitions,"when enabled any admonition found in annotations will be enriched with a corresponding emoji");
  app.add_option("-s,--src-root", option::sroot, "source root directory")
    ->required()
    ->transform(CLI::Validator(&cannonau,"DIR(existing)"));
  app.add_option("-d,--doc-root",option::droot, "document root directory")
    ->required()
    ->transform(CLI::Validator(&cannonau,"DIR(existing)"));
  app.add_option("sources", sources, "directories or files in any combination: paths can be passed either as relative to «Source root» or absolute")
    ->required()
    ->transform(CLI::Validator(
      [] (string &file) -> string {
        auto path = fs::path(file);
        if (path.is_relative()) {
          path  = option::sroot/path;
          file  = path.string();
        }
        return fs::is_regular_file(path)||fs::is_directory(path) ? string() : string("Source path does not exist : ")+path.string();
      }
      ,"PATH(existing)"
    ));
  app.add_flag("-g,--graph",option::graph,"when true, graph generation in document root is enabled");
  app.add_flag("-t,--toc",option::toc,"when true, toc generation in document root is enabled.");
  app.add_option("-i,--ignore-prefix",option::prefix,"prefix to be ignored during ToC sorting");

  try {
    app.parse(argc, argv);
    assert(option::droot.is_absolute());
    assert(option::sroot.is_absolute());

    FileSet src_files;
    lookup(sources,".scad",&src_files);

    // Hide cursor
    indicators::show_console_cursor(false);
    indicators::BlockProgressBar bar{
      indicators::option::BarWidth{50},
      indicators::option::MaxProgress{src_files.size()}
    };
    cout << "Processing " << src_files.size() << " source files:\n";
    {
      scad::Processor proc(new doc::writer::Mdown);
      for(auto file: src_files) {
        // update postfix text with current file working on
        bar.set_option(indicators::option::PostfixText{file});
        proc.document(fs::relative(file,option::sroot));
        bar.tick(); // update progress bar
      }
      bar.set_option(indicators::option::PostfixText{"done."});
      bar.mark_as_completed();
      // Show cursor
      indicators::show_console_cursor(true);
      if (option::toc)
        proc.writeToC();
      if (option::graph)
        proc.writeGraph();
    }
  } catch (const CLI::ParseError &e) {
    result  = app.exit(e);
  } catch(const exception &error) {
    print_exception(error);
    result  = EXIT_FAILURE;
  }
  indicators::show_console_cursor(true);
  return result;
}
