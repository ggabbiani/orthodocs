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
#include <map>
#include <string>

  #ifdef BOOST_TEST_MODULE
  extern int main(int argc, char *argv[]);
  struct ut_custom_datadir;
  #else
  extern int main(int argc, const char *argv[]);
  #endif

/**
 * Convenient namespace for command line passed program options.
 *
 * Actually defines the capability for main() function only to modify these
 * values during argument parsing while providing read-only access to the rest
 * of the world.
 */
namespace cli {

template <typename T> class Opt;
template <typename T> class Map;

namespace _impl {

template <typename T=bool>
class Base {
  #ifdef BOOST_TEST_MODULE
  friend int ::main(int argc, char *argv[]);
  friend struct ::ut_custom_datadir;
  #else
  friend int ::main(int argc, const char *argv[]);
  #endif

  template <typename _T>
  friend class cli::Opt;
  template <typename _T>
  friend class cli::Map;

public:
  using Type = T;

  Base(const std::string &name, const std::string &desc)
  : name{name}, desc{desc} {}

  const std::string name;
  const std::string desc;

  /**
   * function operator acts as value getter
   */
  const Type& operator () () const {
    return value;
  }

private:
  mutable Type value;
};

} // namespace _impl

template <typename T>
struct Opt : public _impl::Base<T> {
  using Super = _impl::Base<T>;
  Opt(const std::string &name, const std::string &desc, T &&def={})
  : Super{name,desc},defaultValue(std::move(def)) {
    this->value = defaultValue;
  }

  const T defaultValue;
};

using Flag = Opt<bool>;

class Version : public Opt<bool> {
  using Opt::Opt;

  /**
   * Version flag actually doesn't hold any value hence no getter
   */
  const Type& operator () () const = delete;
};

template <typename T>
struct Map : public _impl::Base<T> {
  using Super = _impl::Base<T>;
  using Key   = std::string;
  using M     = std::map<Key,T>;
  Map(const std::string &name, const std::string &desc, M &&m, const T &def={})
  : Super{name,desc},map(std::move(m)),defaultValue(value2key(def)) {
    this->value = map.at(defaultValue);
  }
  const M   map;
  // default value is in input (i.e Key) format
  const Key defaultValue;
private:
  // return the unique map key associated to «value»
  Key value2key(T value) {
    for(const auto& [k,v]: this->map)
      if (v==value)
        return k;
    throw std::domain_error("Unmapped value");
  }
};

/**
 * when true, admonitions emoji are enabled for the annotation.
 */
extern const Flag admonitions;
/**
 * data dir in canonical form in the following order of evaluation:
 *
 * * the value passed in the command line if any
 * * ${ODOX_DATADIR_ENV} if ODOX_DATADIR_ENV is set
 * * otherwise ODOX_INSTALL_FULL_DATADIR
 */
extern const Opt<std::filesystem::path> dataDir;
/**
 * defines the annotation prologue string
 */
extern const Opt<std::string> decorations;
/**
 * document root in canonical form
 */
extern const Opt<std::filesystem::path> docRoot;
/**
 * source root in canonical form
 */
extern const Opt<std::filesystem::path> srcRoot;
/**
 * directories or files in any combination: paths can be passed either as
 * relative to «source root» or absolute.
 */
extern const Opt<FileSet> sources;
/**
 * when true, toc generation in document root is enabled
 */
extern const Flag toc;
/**
 * ignore this package prefix in the Table of Contents sort
 */
extern const Opt<std::vector<std::string>> ignorePrefix;
/**
 * if name matches any of the ignored prefixes then abbreviate
 */
extern std::string prefixAbbreviation(const std::string &name);
/**
 * how package dependencies are documented (default "text")
 * Possible values:
 *
 * - text (default)
 * - graph
 * - svg
 */
extern const Opt<std::string> pkg_deps;
/**
 * set of directory for partial dependencies graphs
 */
extern const Opt<FileSet>     graphs;
/**
 * prefix used for private (i.e. not to be documented) IDs (variable,
 * function, module or whatever ...)
 */
extern const Opt<std::string> private_prefix;
/**
 * guess what ...
 */
extern const Version version;

using Verbosity = spdlog::level::level_enum;
/**
 * enables warnings
 */
extern const Map<Verbosity> verbosity;
/**
 * wether to follow orthodox annotation rules or not.
 */
extern const Flag orthodox;
/**
 * defines the language processor to be used.
 * For now only "scad" is supported.
 */
extern const Opt<std::string> language;
/**
 * defines the language processor to be used.
 * For now only "markdown" is supported.
 */
extern const Opt<std::string> writer;

} // namespace cli
