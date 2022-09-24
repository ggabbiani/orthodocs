/*
 * orthodocs: API documentation and static analysis tool
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

#include "orthodocs/analizer.h"
#include "orthodocs/extensions.h"
#include "orthodocs/globals.h"
#include "orthodocs/utils.h"

#include "CLI11.hpp"

#include <algorithm>

using namespace std;
using namespace orthodocs;

namespace fs=std::filesystem;

namespace {

/*
 * transforms an absolute or current working directory relative path in 
 * canonical form
 */
string cwd2canonical(string &dir) {
  dir = fs::canonical(dir).string();
  return string();
}

/**
 * transforms to source root relative.
 */
string sroot_relative(string &sub) {
  string error;
  if (!Option::sroot().empty()) {
    cwd sroot(Option::sroot());
    sub = fs::relative(sub,Option::sroot()).string();
  } else 
    error = "«source tree root» is missing";
  return error;
}

enum {
  ADMONITIONS,
  SRC_ROOT,
  DOC_ROOT,
  SOURCES,
  TOC,
  INGNORE,
  DEPS,
  GRAPHS
};

struct {
  const char *name;
  const char *desc;
} const opt[] = {
  {"-a,--admonitions",    "when enabled any admonition found in annotations will be enriched with a corresponding emoji"},
  {"-s,--src-root",       "source tree root - either an absolute or current directory relative path"                    },
  {"-d,--doc-root",       "document tree root - either an absolute or current directory relative path"             },
  {"sources",             "source sub-trees and/or files - either as absolute or «source tree root» relative path"      },
  {"-t,--toc",            "generate a Table of Content in the document tree root"                                       },
  {"-i,--ignore-prefix",  "ignore this package prefix in the Table of Contents sort"                                    },
  {"--pd,--pkg-deps",     "set package dependecies representation by text list or by a dependency graph (default TEXT)" },
  {"-g,--graphs",         "list of root relative directories where placing graphs"                                      },
};

}

int main(int argc, const char *argv[]) {
  CLI::App  app{"Automatic documentation generation and static analysis tool.","orthodocs"};
  auto      result = EXIT_SUCCESS;

  app.add_flag(   opt[ADMONITIONS].name ,Option::_admonitions  ,opt[ADMONITIONS].desc);
  auto sroot_opt = app.add_option( opt[SRC_ROOT].name    ,Option::_sroot        ,opt[SRC_ROOT].desc)
    ->required()
    ->transform(CLI::Validator(cwd2canonical,"DIR(existing)"));
  app.add_option(opt[DOC_ROOT].name,Option::_droot, opt[DOC_ROOT].desc)
    ->required()
    ->transform(CLI::Validator(cwd2canonical,"DIR(existing)"));
  auto sources_opt = app.add_option(opt[SOURCES].name, Option::_sources, opt[SOURCES].desc)
    ->required()
    ->transform(CLI::Validator(sroot_relative,"PATH(existing)"));
  app.add_flag(opt[TOC].name,Option::_toc,opt[TOC].desc);
  app.add_option(opt[INGNORE].name,Option::_prefix,opt[INGNORE].desc);
  app.add_option(opt[DEPS].name,Option::_pkg_deps,opt[DEPS].desc)
    ->check(CLI::IsMember({"GRAPH", "TEXT"}, CLI::ignore_case));
  auto graph_opt = app.add_option(opt[GRAPHS].name,Option::_graphs,opt[GRAPHS].desc)
    ->transform(CLI::Validator(sroot_relative,"PATH(existing)"));

  sources_opt->needs(sroot_opt);
  graph_opt->needs(sroot_opt);

  try {
    app.parse(argc, argv);
    assert(Option::_droot.is_absolute());
    assert(Option::_sroot.is_absolute());

    // get desired language extension for source analysis
    auto language = language::Extension::factory();
    // build proper analyst
    Analizer analyst(language);
    // in-memory source tree analysis
    analyst.process(Option::sources());
    // get desired writer extension
    auto writer = writer::Extension::factory();
    // save documents
    writer->save(analyst.documents());
    // save table of contents
    if (Option::_toc)
      writer->save(analyst.toc());
    // save graphs
    if (Option::_graphs.size()) 
      writer->graphs(analyst.toc(),Option::_graphs);
      
  } catch (const CLI::Error &error) {
    result  = app.exit(error);
  } catch(const exception &error) {
    print_exception(error);
    result  = EXIT_FAILURE;
  }
  return result;
}
