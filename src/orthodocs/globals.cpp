/*
 * global variables definition
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

#include "debug/trace.h"
#include "orthodocs/globals.h"

using namespace std;
namespace fs=std::filesystem;

bool            Option::_admonitions{false};
fs::path        Option::_droot;
FileSet         Option::_graphs;
string          Option::_language{"scad"};
string          Option::_pkg_deps{"text"};
vector<string>  Option::_ignore_prefix;
string          Option::_private_prefix{"__"};
bool            Option::_quiet{false};
FileSet         Option::_sources;
fs::path        Option::_sroot;
bool            Option::_toc{false};
string          Option::_writer{"markdown"};

string Option::prefix_abbreviation(const string &name) {
  TR_FUNC;
  string  abbreviated = name;
  for(const auto &pfx: _ignore_prefix) {
    auto    len = pfx.length();
    if (nocase::equal(name.substr(0,len),pfx)) {
      abbreviated = name.substr(len);
      TR_MSG(name,"matched",pfx);
      break;
    }
  }
  return abbreviated;
}
