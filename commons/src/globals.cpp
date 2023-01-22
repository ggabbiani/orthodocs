/*
 * global variables definition
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later */

#include "globals.h"

using namespace std;
namespace fs=std::filesystem;

bool              Option::_admonitions{false};
fs::path          Option::_droot;
FileSet           Option::_graphs;
vector<string>    Option::_ignore_prefix;
string            Option::_language{"scad"};
string            Option::_pkg_deps{"text"};
string            Option::_private_prefix{"__"};
FileSet           Option::_sources;
fs::path          Option::_sroot;
bool              Option::_toc{false};
Option::Verbosity Option::_verbosity{Option::Verbosity::err};
string            Option::_writer{"markdown"};

string Option::prefix_abbreviation(const string &name) {
  string  abbreviated = name;
  for(const auto &pfx: _ignore_prefix) {
    auto    len = pfx.length();
    if (nocase::equal(name.substr(0,len),pfx)) {
      abbreviated = name.substr(len);
      break;
    }
  }
  return abbreviated;
}
