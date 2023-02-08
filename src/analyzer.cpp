/*
 * source file analysis
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "analyzer.h"

#include <commons/bar.h>
#include <commons/error_info.h>
#include <commons/extensions.h>
#include <commons/globals.h>
#include <commons/spdx.h>
#include <commons/utils.h>

using namespace std;

namespace fs = std::filesystem;

std::filesystem::path subpath(const std::filesystem::path &path, std::filesystem::path::iterator from) {
  std::filesystem::path result;
  for(auto &i=from; i!=path.end(); ++i) {
    result /= *i;
  }
  return result;
}

void Analyzer::document(fs::path source) {
  assert(source.is_relative());
  assert(source.has_filename());
  if (source.begin()!=source.end() && *source.begin()==".")
    source = subpath(source,++source.begin());
  try {
    auto document = _language->parse(source);
    // copy document contents to the Table of Contents
    doc::toc::add(document.get(),_toc);
    // move document in the documents list
    _docs.emplace(_docs.end(),std::move(document));
  } catch(...) {
    throw_with_nested(runtime_error(ERR_CALL(source)));
  }
}

auto Analyzer::buildDocuments() -> void {
  try {
    FileSet files;
    lookup(Option::sources().size() ? Option::sources() : FileSet{"."}, _language->sourcePostfix(),files);
    Bar bar(files,"sources analyzed");
    for(const auto &file: files) {
      bar.status(file.string());
      document(file);
      bar++;
    }
  } catch(...) {
    indicators::show_console_cursor(true);
    throw;
  }
}

auto Analyzer::populate() const -> Dictionary {
  try {
    Dictionary dict;
    Bar bar(_toc, "item(s) in the xref dictionary");
    for(auto &item: _toc) {
      auto key = item->dictKey;
      bar.status(key);
      if (auto [i, success] = dict.try_emplace(key,item); !success) {
        // TODO: eventually manage fully qualified referenced items
        spdlog::warn("duplicate item '{}' skipped",key);
      }
      bar++;
    }
    return dict;
  } catch(...) {
    indicators::show_console_cursor(true);
    throw;
  }
}

void Analyzer::xref() const {
  // setup array of non empty annotations
  vector<doc::Annotation*> annos;
  for(const auto& document: _docs) {
    for(auto &item: document->index) {
      if (!item->annotation.empty())
        annos.push_back(&item->annotation);
      for_each(item->parameters.begin(),item->parameters.end(),[&annos](const doc::Parameter::Owner &p) {
        if (!p->annotation.empty())
          annos.push_back(&p->annotation);
      });
    }
  }
  try {
    Bar bar(annos, "annotations analyzed");
    for_each(annos.begin(),annos.end(),[&bar,this] (doc::Annotation *a) {
      _language->analyze(*a);
      spdx::analyze(*a);
      bar++;
    });
  } catch(...) {
    indicators::show_console_cursor(true);
    throw;
  }
}

void Analyzer::lookup(const FileSet &sources, const char *extension, FileSet &result) {
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
      throw domain_error(ERR_INFO+"what is this '"+path.string()+"'?");
  }
}
