#pragma once
/*
 * globals variables from cli.
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <commons/utils.h>

#include <spdlog/spdlog.h>
#include <filesystem>
#include <string>

/**
 * Convenient namespace for command line passed program options.
 *
 * Actually defines the capability for main() function only to modify these
 * values during argument parsing while providing read-only access to the rest
 * of the world.
 */
class Option {
  #ifdef BOOST_TEST_MODULE
  friend int main(int argc, char *argv[]);
  #else
  friend int main(int argc, const char *argv[]);
  #endif
public:
  using Verbosity = spdlog::level::level_enum;

  /**
   * when true, adminitions emoji are enabled for the annotation.
   */
  static bool admonitions() {return _admonitions;}
  /**
   * document root in canonical form
   */
  static const std::filesystem::path &droot() {return _droot;}
  /**
   * defines the annotation prologue string
   */
  static const std::string &annotationProlog() {return _annotation_prolog;}
  /**
   * set of directory for partial dependencies graphs
   */
  static const FileSet &graphs() {return _graphs;}
  /**
   * defines the language processor to be used.
   * For now only "scad" is supported.
   */
  static const std::string &language() {return _language;}
  /**
   * how package dependecies are documented (default "text")
   */
  static const std::string &pkg_deps() {return _pkg_deps;}
  /**
   * ignore this package prefix in the Table of Contents sort
   */
  static const std::vector<std::string> &ignore_prefix() {return _ignore_prefix;}
  /**
   * wether to follow orthodox annotation rules or not.
   */
  static bool &orthodox() {return _orthodox;}
  /**
   * if name matches any of the ignored prefixes then abbreviate
   */
  static std::string prefix_abbreviation(const std::string &name);
  /**
   * prefix used for private (i.e. not to be documented) IDs (variable,
   * function, module or whatever ...)
   */
  static const std::string &private_prefix() {return _private_prefix;}
  /**
   * directories or files in any combination: paths can be passed either as
   * relative to «Source root» or absolute.
   */
  static const FileSet &sources() {return _sources;}
  /**
   * source root in canonical form
   */
  static const std::filesystem::path &sroot() {return _sroot;}
  /**
   * when true, toc generation in document root is enabled
   */
  static bool toc() {return _toc;}
  /**
   * defines the language processor to be used.
   * For now only "markdown" is supported.
   */
  static const std::string &writer() {return _writer;}
  /**
   * enables warnings
   */
  static Verbosity verbosity() {return _verbosity;}
private:
  static bool                     _admonitions;
  static std::string              _annotation_prolog;
  static std::filesystem::path    _droot;
  static FileSet                  _graphs;
  static std::vector<std::string> _ignore_prefix;
  static std::string              _language;
  static bool                     _orthodox;
  static std::string              _pkg_deps;
  static std::string              _private_prefix;
  static FileSet                  _sources;
  static std::filesystem::path    _sroot;
  static bool                     _toc;
  static Verbosity                _verbosity;
  static std::string              _writer;
};
