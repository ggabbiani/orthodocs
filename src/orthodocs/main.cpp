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

#include "error_info.h"
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
 * set existing «dir» in canonical form
 */
string existing_canonical_dir(string &dir) {
  try {
    fs::path p(dir);
    if (!fs::exists(p)) {
      return "Directory does not exist: " + dir;
    } else if (fs::is_regular_file(p)) {
      return  "Directory is actually a file: " + dir;
    }
    dir = fs::canonical(p).string();
    return string();
  } catch(const fs::filesystem_error &error) {
    return error.what();
  } catch(...) {
    throw_with_nested(runtime_error(ERR_CALL("\""+dir+"\"")));
  }
}

string canonical_dir(string &dir) {
  try {
    fs::path p(dir);
    if (!fs::exists(p)) 
      fs::create_directories(p);
    if (!fs::is_directory(p))
      return  "If existing, must be a directory: " + dir;
    dir = fs::canonical(p).string();
    return string();
  } catch(const fs::filesystem_error &error) {
    return error.what();
  } catch(...) {
    throw_with_nested(runtime_error(ERR_CALL("\""+dir+"\"")));
  }
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
  DOC_ROOT,
  GRAPHS,
  IGNORE,
  PRIVATE,
  DEPS,
  QUIET,
  SOURCES,
  SRC_ROOT,
  TOC,
};

struct {
  const char *name;
  const char *desc;
} const opt[] = {
  {"-a,--admonitions",    "when enabled any admonition found in annotations will be enriched with a corresponding emoji"},
  {"-d,--doc-root",       "document tree root - either an absolute or current directory relative path"                  },
  {"-g,--graphs",         "list of root relative directories where placing graphs"                                      },
  {"-i,--ignore-prefix",  "ignore this package prefix in the Table of Contents sort"                                    },
  {"-p,--private",        "prefix used for private (not to be documented) IDs (variable, function, module or whatever)" },
  {"--pd,--pkg-deps",     "set package dependecies representation by text list or by a dependency graph (default TEXT)" },
  {"-q,--quiet",          "quiet mode"},
  {"sources",             "source sub-trees and/or files - either as absolute or «source tree root» relative path"      },
  {"-s,--src-root",       "source tree root - either an absolute or current directory relative path"                    },
  {"-t,--toc",            "generate a Table of Contents in the document tree root"                                       },
};

}

int main(int argc, const char *argv[]) {
    CLI::App  app{"Automatic documentation generation and static analysis tool.","orthodocs"};
    auto      result = EXIT_SUCCESS;

  try {
    app.add_flag(   opt[ADMONITIONS].name ,Option::_admonitions  ,opt[ADMONITIONS].desc);
    auto sroot_opt = app.add_option( opt[SRC_ROOT].name    ,Option::_sroot        ,opt[SRC_ROOT].desc)
      ->required()
      ->transform(CLI::Validator(existing_canonical_dir,"DIR(existing)"));
    app.add_option(opt[DOC_ROOT].name,Option::_droot, opt[DOC_ROOT].desc)
      ->required()
      ->transform(CLI::Validator(canonical_dir,"DIR"));
    auto sources_opt = app.add_option(opt[SOURCES].name, Option::_sources, opt[SOURCES].desc)
      ->required()
      ->transform(CLI::Validator(sroot_relative,"PATH(existing)"));
    app.add_flag(opt[TOC].name,Option::_toc,opt[TOC].desc);
    app.add_option(opt[IGNORE].name,Option::_ignore_prefix,opt[IGNORE].desc);
    app.add_option(opt[DEPS].name,Option::_pkg_deps,opt[DEPS].desc)
      ->check(CLI::IsMember({"GRAPH", "TEXT"}, CLI::ignore_case));
    auto graph_opt = app.add_option(opt[GRAPHS].name,Option::_graphs,opt[GRAPHS].desc)
      ->transform(CLI::Validator(sroot_relative,"PATH(existing)"));
    app.add_option(opt[PRIVATE].name, Option::_private_prefix, opt[PRIVATE].desc);
    app.add_flag(opt[QUIET].name,Option::_quiet,opt[QUIET].desc);

    sources_opt->needs(sroot_opt);
    graph_opt->needs(sroot_opt);

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
