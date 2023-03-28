/*
 * orthodocs: API documentation and static analysis tool
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "analyzer.h"

// project components
#include <commons/config.h>
#include <commons/error_info.h>
#include <commons/exceptions.h>
#include <commons/extensions.h>
#include <commons/globals.h>
#include <commons/utils.h>

// third party
#include <CLI/CLI.hpp>

// system
#include <algorithm>
#include <cassert>
#include <map>

#if defined(_WIN32)
#include <windows.h>
#endif  // defined(_WIN32)

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
  FLG_ADMONITIONS,
  OPT_DATA_DIR,
  OPT_DECORATIONS,
  OPT_DOC_ROOT,
  OPT_GRAPHS,
  OPT_IGNORE,
  OPT_PRIVATE,
  OPT_DEPS,
  FLG_ORTHODOX,
  OPT_SOURCES,
  OPT_SRC_ROOT,
  FLG_TOC,
  OPT_VERBOSITY,
  FLG_VERSION,
};

struct {
  const char *name;
  const char *desc;
} const opt[] = {
  {"-a,--admonitions",            "when enabled any admonition found in annotations will be enriched with a corresponding emoji"},
  {"--data-dir",                  "absolute path to data directory: touch only if you know what you do"                         },
  {"--decorations",               "defines the prefix used inside comments to distinguish a simple comment from an annotation"
                                  " Set to '' means no decorations, and all the comments are interpreted as annotations."       },
  {"-d,--doc-root",               "document tree root - either an absolute or current directory relative path"                  },
  {"-g,--graphs",                 "list of root relative directories where placing graphs"                                      },
  {"-i,--ignore-prefix",          "ignore this package prefix in the Table of Contents sort"                                    },
  {"-p,--private",                "prefix used for private (not to be documented) IDs (variable, function, module or whatever)" },
  {"--pd,--pkg-deps",             "set package dependencies representation by text list or by a dependency graph (default TEXT)"},
  {"-o,--orthodox,!--unorthodox", "comments for parameters are preceding their formal definition"                               },
  {"sources",                     "«source root» sub-trees and/or files - either as absolute or «source root» relative path."
                                  " If missing all the «source root» content sources will be scanned"                           },
  {"-s,--src-root",               "source tree root - either an absolute or current directory relative path"                    },
  {"-t,--toc",                    "generate a Table of Contents in the document tree root"                                      },
  {"-V,--verbosity",              "set spdlog verbosity"                                                                        },
  {"-v,--version",                "orthodocs version " ODOX_VERSION_STR                                                         },
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

#if defined(_WIN32)
  HANDLE hOutput  = INVALID_HANDLE_VALUE;
  DWORD oldDwMode = 0;
#endif  // defined(_WIN32)

  try {
#if defined(_WIN32)
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOutput==INVALID_HANDLE_VALUE)
      throw OsError("GetStdHandle() failed");

    if (GetConsoleMode(hOutput, &oldDwMode)) {
      if (!SetConsoleMode(hOutput, oldDwMode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
        throw OsError("SetConsoleMode() failed");
    } else {
      spdlog::warn("GetConsoleMode() failed");
    }
#endif  // defined(_WIN32)

    spdlog::set_pattern("%^[%l]%$ %v");

    app.add_flag(   opt[FLG_ADMONITIONS].name ,Option::_admonitions  ,opt[FLG_ADMONITIONS].desc);
    auto sroot_opt = app.add_option( opt[OPT_SRC_ROOT].name    ,Option::_sroot        ,opt[OPT_SRC_ROOT].desc)
      ->required()
      ->transform(CLI::Validator(existing_canonical_dir,"DIR(existing)"));
    app.add_option(opt[OPT_DECORATIONS].name, Option::_decorations, opt[OPT_DECORATIONS].desc)
      ->default_val("!");
    app.add_option(opt[OPT_DOC_ROOT].name,Option::_droot, opt[OPT_DOC_ROOT].desc)
      ->required()
      ->transform(CLI::Validator(canonical_dir,"DIR"));
    auto sources_opt = app.add_option(opt[OPT_SOURCES].name, Option::_sources, opt[OPT_SOURCES].desc)
      ->transform(CLI::Validator(sroot_relative,"PATH(existing)"));
    app.add_flag(opt[FLG_TOC].name,Option::_toc,opt[FLG_TOC].desc);
    app.add_option(opt[OPT_IGNORE].name,Option::_ignore_prefix,opt[OPT_IGNORE].desc);
    app.add_option(opt[OPT_DEPS].name,Option::_pkg_deps,opt[OPT_DEPS].desc)
      ->check(CLI::IsMember({"GRAPH", "TEXT", "SVG"}, CLI::ignore_case));
    auto graph_opt = app.add_option(opt[OPT_GRAPHS].name,Option::_graphs,opt[OPT_GRAPHS].desc)
      ->transform(CLI::Validator(sroot_relative,"PATH(existing)"));
    app.add_option(opt[OPT_PRIVATE].name, Option::_private_prefix, opt[OPT_PRIVATE].desc)
      ->default_val("__");
    app.set_version_flag(opt[FLG_VERSION].name, opt[FLG_VERSION].desc);
    app.add_option(opt[OPT_VERBOSITY].name,Option::_verbosity,opt[OPT_VERBOSITY].desc)
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
      ->default_val("error");
    app.add_flag(opt[FLG_ORTHODOX].name, Option::_orthodox,opt[FLG_ORTHODOX].desc)
      ->default_val(true);
    app.add_option(opt[OPT_DATA_DIR].name, Option::_data_dir, opt[OPT_DATA_DIR].desc)
      ->check(CLI::ExistingDirectory);

    sources_opt->needs(sroot_opt);
    graph_opt->needs(sroot_opt);

    app.parse(argc, argv);
    assert(Option::_droot.is_absolute());
    assert(Option::_sroot.is_absolute());
    spdlog::set_level(Option::verbosity());

    // desired language extension for source analysis
    auto language = language::Extension::factory(Option::language());
    // language analyst setup
    Analyzer analyst(language);
    // in-memory source tree analysis production
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
  } catch (const CLI::Success &message) {
    // typically version or help messages: these are managed internally by CLI
    // and consequently printed on terminal during app.exit().
    result  = app.exit(message);
  } catch (const CLI::Error &error) {
    // CLI parsing errors
    print_exception(error);
    result  = app.exit(error);
  } catch(const RcException &error) {
    // OrthoDocs errors: return code is embedded
    print_exception(error);
    result  = error.rc;
  } catch(const exception &error) {
    // system errors
    print_exception(error);
    result  = EXIT_FAILURE;
  }
#if defined(_WIN32)
  // Restore input mode on exit.
  if (oldDwMode)
    SetConsoleMode(hOutput, oldDwMode);
#endif  // defined(_WIN32)
  return result;
}
