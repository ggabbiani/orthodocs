#pragma once
/*
 * globals variables from cli.
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

#include "singleton.h"
#include "utils.h"

#include <filesystem>
#include <string>

class Option {
  friend int main(int argc,const char *argv[]);
public:
  /**
   * document root in canonical form
   */
  static const std::filesystem::path &droot() {return _droot;}
  /**
   * source root in canonical form
   */
  static const std::filesystem::path &sroot() {return _sroot;}
  /**
   * when true, adminitions emoji are enabled for the annotation.
   */
  static bool admonitions() {return _admonitions;}
  /**
   * when true, toc generation in document root is enabled
   */
  static bool toc() {return _toc;}
  static const std::string &prefix() {return _prefix;}
  /**
   * how package dependecies are documented (default "text")
   */
  static const std::string &pkg_deps() {return _pkg_deps;}
  /**
   * set of directory for partial dependencies graphs
   */
  static const FileSet &graphs() {return _graphs;}
  /**
   * directories or files in any combination: paths can be passed either as relative to «Source root» or absolute.
   */
  static const FileSet &sources() {return _sources;}
  /**
   * defines the language processor to be used. 
   * For now only "scad" is supported.
   */
  static const std::string &language() {return _language;}
  /**
   * defines the language processor to be used. 
   * For now only "markdown" is supported.
   */
  static const std::string &writer() {return _writer;}
private:
  static std::filesystem::path _droot;
  static std::filesystem::path _sroot;
  static bool                  _admonitions;
  static bool                  _toc;
  static std::string           _prefix;
  static std::string           _pkg_deps;
  static FileSet               _graphs;
  static FileSet               _sources;
  static std::string           _language;
  static std::string           _writer;
};
