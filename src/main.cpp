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
  if (cli::srcRoot().empty())
    return "«source tree root» is missing";
  if (fs::path(sub).is_absolute())
    sub = fs::relative(sub,cli::srcRoot()).make_preferred().generic_string();
  cwd sroot(cli::srcRoot());
  if (!fs::exists(sub))
    return "«"+sub+"» not existing";
  return "";
}

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

    app.add_flag(cli::admonitions.name ,cli::admonitions.value ,cli::admonitions.desc);
    auto sroot_opt = app.add_option(cli::srcRoot.name, cli::srcRoot.value, cli::srcRoot.desc)
      ->required()
      ->transform(CLI::Validator(existing_canonical_dir,"DIR(existing)"));
    app.add_option(cli::decorations.name, cli::decorations.value, cli::decorations.desc)
      ->default_val(cli::decorations.defaultValue);
    app.add_option(cli::docRoot.name, cli::docRoot.value, cli::docRoot.desc)
      ->required()
      ->transform(CLI::Validator(canonical_dir,"DIR"));
    auto sources_opt = app.add_option(cli::sources.name, cli::sources.value, cli::sources.desc)
      ->transform(CLI::Validator(sroot_relative,"PATH(existing)"));
    app.add_flag(cli::toc.name, cli::toc.value, cli::toc.desc);
    app.add_option(cli::ignorePrefix.name,cli::ignorePrefix.value, cli::ignorePrefix.desc);
    app.add_option(cli::pkg_deps.name, cli::pkg_deps.value, cli::pkg_deps.desc)
      ->check(CLI::IsMember({"graph", "text", "svg"}, CLI::ignore_case))
      ->default_val(cli::pkg_deps.defaultValue);
    auto graph_opt = app.add_option(cli::graphs.name, cli::graphs.value, cli::graphs.desc)
      ->transform(CLI::Validator(sroot_relative,"PATH(existing)"));
    app.add_option(cli::private_prefix.name, cli::private_prefix.value, cli::private_prefix.desc)
      ->default_val(cli::private_prefix.defaultValue);
    app.set_version_flag(cli::version.name, cli::version.desc);
    app.add_option(cli::verbosity.name,cli::verbosity.value,cli::verbosity.desc)
      ->transform(CLI::CheckedTransformer(cli::verbosity.map))
      ->default_val(cli::verbosity.defaultValue);
    app.add_flag(cli::orthodox.name, cli::orthodox.value, cli::orthodox.desc)
      ->default_val(cli::orthodox.defaultValue);
    app.add_option(cli::dataDir.name, cli::dataDir.value, cli::dataDir.desc)
      ->check(CLI::ExistingDirectory);

    sources_opt->needs(sroot_opt);
    graph_opt->needs(sroot_opt);

    app.parse(argc, argv);
    assert(cli::docRoot().is_absolute());
    assert(cli::srcRoot().is_absolute());
    spdlog::set_level(cli::verbosity());

    // in debug mode print some diagnosis infos
    spdlog::debug("Data dir: '{0}'",cli::dataDir.value.string());

    // desired language extension for source analysis
    auto language = language::Extension::factory(cli::language());
    // language analyst setup
    Analyzer analyst(language);
    // in-memory source tree analysis production
    analyst.buildDocuments();
    // populated cross-reference dictionary
    doc::xref::Dictionary  dict = analyst.populate();
    // gather cross-reference data from all the annotations
    analyst.xref();
    // desired writer extension
    auto writer = writer::Extension::factory(cli::writer(),dict,language);
    // save documents
    writer->save(analyst.documents());
    // save table of contents
    if (cli::toc())
      writer->save(analyst.toc());
    // save graphs
    if (cli::graphs().size())
      writer->graphs(analyst.toc(),cli::graphs());
  } catch (const CLI::Error &error) {
    // CLI parsing errors
    result  = app.exit(error);
  } catch(const RcException &error) {
    // OrthoDocs errors: return code is embedded
    exceptions::print(error);
    result  = error.rc;
  } catch(const exception &error) {
    // system errors
    exceptions::print(error);
    result  = EXIT_FAILURE;
  }

#if defined(_WIN32)
  // Restore input mode on exit.
  if (oldDwMode)
    SetConsoleMode(hOutput, oldDwMode);
#endif  // defined(_WIN32)
  return result;
}
