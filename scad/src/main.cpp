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

/*
 * transforms an absolute or current working directory relative path in 
 * canonical form
 */
string cwd2canonical(string &dir) {
  // cwd pwd(dir);
  dir = fs::canonical(dir).string();
  return string();
}

/**
 * transforms to source root relative.
 */
string sroot_relative(string &sub) {
  sub = fs::relative(sub,option::sroot).string();
  return string();
}

int main(int argc, const char *argv[]) {
  CLI::App  app{"Automatic documentation generation for the OpenSCAD language.","adox-scad"};
  auto      result = EXIT_SUCCESS;

  app.add_flag("-a,--admonitions",option::admonitions,"when enabled any admonition found in annotations will be enriched with a corresponding emoji");
  app.add_option("-s,--src-root", option::sroot, "source root directory: either as absolute or current directory relative path")
    ->required()
    ->transform(CLI::Validator(cwd2canonical,"DIR(existing)"));
  app.add_option("-d,--doc-root",option::droot, "document root directory - either as absolute or current directory relative path")
    ->required()
    ->transform(CLI::Validator(cwd2canonical,"DIR(existing)"));
  app.add_option("sources", option::sources, "source directories and files: either as absolute or «Source root» relative path")
    ->required()
    ->transform(CLI::Validator(sroot_relative,"PATH(existing)"));
  app.add_flag("-t,--toc",option::toc,"when true, toc generation in document root is enabled.");
  app.add_option("-i,--ignore-prefix",option::prefix,"prefix to be ignored during ToC sorting");
  app.add_option("--pd,--pkg-deps",option::pkg_deps,"how package dependecies are documented (default TEXT)")
    ->check(CLI::IsMember({"GRAPH", "TEXT"}, CLI::ignore_case));
  app.add_option("-g,--graphs",option::graphs,"set of root relative directories for graphs")
    ->transform(CLI::Validator(sroot_relative,"PATH(existing)"));

  try {
    app.parse(argc, argv);
    assert(option::droot.is_absolute());
    assert(option::sroot.is_absolute());

    FileSet src_files;
    lookup(option::sources,".scad",&src_files);

    // Hide cursor
    indicators::show_console_cursor(false);
    indicators::BlockProgressBar bar{
      indicators::option::BarWidth{50},
      indicators::option::MaxProgress{src_files.size()}
    };
    cout << "Processing " << src_files.size() << " source files:\n";
    {
      scad::Processor proc(new doc::writer::Mdown);
      for(const auto &file: src_files) {
        // update postfix text with current file working on
        bar.set_option(indicators::option::PostfixText{file});
        proc.document(file);
        bar.tick(); // update progress bar
      }
      bar.set_option(indicators::option::PostfixText{"done."});
      bar.mark_as_completed();
      // Show cursor
      indicators::show_console_cursor(true);
      if (option::toc)
        proc.writeToC();
      if (option::graphs.size()) 
        proc.writeGraphs(option::graphs);
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
