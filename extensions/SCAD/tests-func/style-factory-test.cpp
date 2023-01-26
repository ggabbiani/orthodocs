/*
 * Functional test of class scad::doc::style::Factory capability to recognize
 * the comment style used inside an annotation.
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <commons/globals.h>
#include <commons/utils.h>
#include <scad/document.h>

#include <CLI/CLI.hpp>

#include <fstream>
#include <string>
#include <string.h>

using namespace std;

namespace fs=std::filesystem;

void lookup(const FileSet &sources, const char *extension, FileSet &result) {
  cwd pwd(Option::sroot());
  for(auto &path: sources) {
    if (fs::is_regular_file(path)) {
      if (!extension || path.extension()==extension)
        result.push_back(path);
    } else if (fs::is_directory(path)) {
      for (auto &entry: fs::directory_iterator{path}) {
        const auto &entry_path = entry.path();
        if (fs::is_regular_file(entry_path)) {
          if (!extension || entry_path.extension()==extension)
            result.push_back(entry_path);
        } else if (fs::is_directory(entry_path)) {
          lookup(FileSet{entry_path},extension,result);
        }
      }
    } else
      throw domain_error("what is this '"+path.string()+"'?");
  }
}

int main(int argc, const char *argv[]) {
  CLI::App app{"style-tester: outputs the checked annotation text or the detected annotation styles from the passed .anno files."};
  auto      result = EXIT_SUCCESS;
  fs::path  sroot;
  string    expected;

  app.add_option("-s,--src-root", sroot, "Source root directory")
    ->required()
    ->check(CLI::ExistingDirectory);
  app.add_option("sources", Option::_sources, "Directories or files in any combination: paths can be passed either as relative to «Source root» or absolute")
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
  app.add_option("-e,--expected",expected,"Expected annotation type, when empty only the resulted annotation style will be reported")
    ->check(CLI::IsMember({"SINGLE","SIMPLE","FINE"}, CLI::ignore_case));

  try {
    app.parse(argc, argv);

    // lookup for annotation files (*.anno)
    FileSet src_files;
    lookup(Option::sources().size() ? Option::sources() : FileSet{"."},".anno",src_files);

    string annotation;
    string fname;
    // just one reusable input stream
    ifstream in;
    for(auto file: src_files) {
      static char buffer[256];
      in.open(file);
      fname = file.filename().string();

      // getline() method consumes the ending newline, so we have to re-insert
      auto line = 1;
      while (in.getline(buffer, sizeof buffer)) {
        annotation.append(string(buffer)+'\n');
        if (line==1)  // skip leading spaces in first row
          annotation = annotation.substr(annotation.find('/'));
        ++line;
      }

      // detected annotation style
      scad::doc::AbstractStyle *style;
      try {
        style  = scad::doc::style::Factory()(annotation);
      } catch (runtime_error &error) {
        // we catch and re-throw after contextualizing the error message with the file name
        throw runtime_error(fname+": "+error.what());
      }
      if (expected.empty()) { // detected style report only
        cout << fname << ": " << style->id() << endl;
      } else { // full check and report
        if (expected.compare(style->id())!=0)
          throw runtime_error("File "+fname+": expected "+expected+" got "+style->id());
        cout  << "          1         2         3         4" << endl
              << "01234567890123456789012345678901234567890" << endl
              << style->manage(annotation) << endl
              << "          1         2         3         4" << endl
              << "01234567890123456789012345678901234567890" << endl;
      }

      in.close();
      annotation.clear();
    }

  } catch (const CLI::ParseError &error) {
    result  = app.exit(error);
  } catch(const exception &error) {
    cerr << error.what() << endl;
    result  = EXIT_FAILURE;
  }
  return result;

}