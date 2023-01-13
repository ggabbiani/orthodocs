/*
 * orthodocs: API documentation and static analysis tool
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "analizer.h"

#include <config.h>
#include <error_info.h>
#include <extensions.h>
#include <globals.h>
#include <utils.h>

#include <CLI/CLI.hpp>

#include <algorithm>
#include <cassert>
#include <map>

using namespace std;

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
  SOURCES,
  SRC_ROOT,
  TOC,
  VERBOSITY,
  VERSION,
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
  {"sources",             "source root sub-trees and/or files - either as absolute or «source tree root» relative path."
                          " If missing all the source root will be scanned"                                             },
  {"-s,--src-root",       "source tree root - either an absolute or current directory relative path"                    },
  {"-t,--toc",            "generate a Table of Contents in the document tree root"                                      },
  {"-V,--verbosity",      "set spdlog verbosity"                                                                        },
  {"-v,--version",        "orthodocs version " ODOX_VERSION_STR                                                         },
};

std::underlying_type_t<spdlog::level::level_enum> to_logLevel(std::string_view s) {
  if      (s=="trace")    return to_underlying(spdlog::level::trace);
  else if (s=="debug")    return to_underlying(spdlog::level::debug);
  else if (s=="info")     return to_underlying(spdlog::level::info);
  else if (s=="warn")     return to_underlying(spdlog::level::warn);
  else if (s=="error")    return to_underlying(spdlog::level::err);
  else if (s=="critical") return to_underlying(spdlog::level::critical);
  else if (s=="off")      return to_underlying(spdlog::level::off);
  else throw(domain_error(ERR_INFO+"unknown spdlog::level enumeration '"+string(s)+'\''));
}

/**
 */
struct LogLevelLess {
  using is_transparent = void;
  inline bool operator() (std::string_view lhs, std::string_view rhs) const {
    return to_logLevel(lhs)<to_logLevel(rhs);
  }
};

}

int main(int argc, const char *argv[]) {
  CLI::App  app{"Automatic documentation generation and static analysis tool.","orthodocs"};
  auto      result = EXIT_SUCCESS;

  try {
    spdlog::set_pattern("%^[%l]%$ %v");

    app.add_flag(   opt[ADMONITIONS].name ,Option::_admonitions  ,opt[ADMONITIONS].desc);
    auto sroot_opt = app.add_option( opt[SRC_ROOT].name    ,Option::_sroot        ,opt[SRC_ROOT].desc)
      ->required()
      ->transform(CLI::Validator(existing_canonical_dir,"DIR(existing)"));
    app.add_option(opt[DOC_ROOT].name,Option::_droot, opt[DOC_ROOT].desc)
      ->required()
      ->transform(CLI::Validator(canonical_dir,"DIR"));
    auto sources_opt = app.add_option(opt[SOURCES].name, Option::_sources, opt[SOURCES].desc)
      ->transform(CLI::Validator(sroot_relative,"PATH(existing)"));
    app.add_flag(opt[TOC].name,Option::_toc,opt[TOC].desc);
    app.add_option(opt[IGNORE].name,Option::_ignore_prefix,opt[IGNORE].desc);
    app.add_option(opt[DEPS].name,Option::_pkg_deps,opt[DEPS].desc)
      ->check(CLI::IsMember({"GRAPH", "TEXT"}, CLI::ignore_case));
    auto graph_opt = app.add_option(opt[GRAPHS].name,Option::_graphs,opt[GRAPHS].desc)
      ->transform(CLI::Validator(sroot_relative,"PATH(existing)"));
    app.add_option(opt[PRIVATE].name, Option::_private_prefix, opt[PRIVATE].desc);
    app.set_version_flag(opt[VERSION].name, opt[VERSION].desc);
    app.add_option(opt[VERBOSITY].name,Option::_verbosity,opt[VERBOSITY].desc)
      ->transform(
        CLI::CheckedTransformer(
          map<string, spdlog::level::level_enum, LogLevelLess >(
            {
              {"trace",     spdlog::level::trace    }, 
              {"debug",     spdlog::level::debug    },
              {"info",      spdlog::level::info     },
              {"warn",      spdlog::level::warn     },
              {"error",     spdlog::level::err      },
              {"critical",  spdlog::level::critical },
              {"off",       spdlog::level::off      }
            }
          )))
      ->default_val("info");

    sources_opt->needs(sroot_opt);
    graph_opt->needs(sroot_opt);

    app.parse(argc, argv);
    assert(Option::_droot.is_absolute());
    assert(Option::_sroot.is_absolute());
    spdlog::set_level(Option::verbosity());

    // desired language extension for source analysis
    auto language = language::Extension::factory(Option::language());
    // language analyst setup
    Analizer analyst(language);
    // in-memory source tree analysis prodution
    analyst.buildDocuments();
    // populated cross-reference dictionary 
    doc::xref::Dictionary  dict = analyst.populate();
    // gather cross-reference data from all the annotations
    analyst.xref();
    // desired writer extension
    auto writer = writer::Extension::factory(Option::writer(),dict,language);
    // save documents
    writer->save(analyst.documents());
    // save table of contents
    if (Option::toc())
      writer->save(analyst.toc());
    // save graphs
    if (Option::graphs().size())
      writer->graphs(analyst.toc(),Option::graphs());
  } catch (const CLI::Error &error) {
    result  = app.exit(error);
  } catch(const exception &error) {
    print_exception(error);
    result  = EXIT_FAILURE;
  }
  return result;
}
