/*
 * global variables definition
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <commons/config.h>
#include <commons/globals.h>

#include <cstdlib>

using namespace std;
namespace fs=std::filesystem;

namespace cli {

const Flag admonitions{
  // name
  "-a,--admonitions",
  // description
  "when enabled any admonition found in annotations will be enriched with a"
  " corresponding emoji"
};

const Opt<fs::path> dataDir{
  // name
  "--data-dir",
  // description
  "path to data directory, when not passed is retrieved from the environment"
  " variable '" ODOX_DATADIR_ENV "'"
};

const Opt<std::string> decorations{
  // name
  "--decorations",
  // description
  "prefix used inside comments to distinguish a simple comment from an annotation."
  " Set to '' means no decorations, and all the comments are interpreted as"
  " annotations.",
  // default
  "!"
};

const Opt<fs::path> docRoot{
  // name
  "-d,--doc-root",
  // description
  "document tree root - either an absolute or current directory relative path"
};

const Opt<fs::path> srcRoot{
  // name
  "-s,--src-root",
  // description
  "source tree root - either an absolute or current directory relative path"
};

const Opt<FileSet> sources{
  // name
  "sources",
  // description
  "«source root» sub-trees and/or files - either as absolute or «source root»"
  " relative path. If missing all the «source root» content sources will be"
  " scanned"
};

const Flag toc{
  // name
  "-t,--toc",
  // description
  "generate a Table of Contents in the document tree root"
};

const Opt<vector<string>> ignorePrefix{
  // name
  "-i,--ignore-prefix",
  // description
  "ignore this package prefix in the Table of Contents sort"
};

string prefixAbbreviation(const string &name) {
  string  abbreviated = name;
  for(const auto &pfx: ignorePrefix()) {
    auto    len = pfx.length();
    if (nocase::equal(name.substr(0,len),pfx)) {
      abbreviated = name.substr(len);
      break;
    }
  }
  return abbreviated;
}

const Opt<string> pkg_deps{
  // name
  "--pd,--pkg-deps",
  // description
  "set package dependencies representation by text list or by a dependency graph"
  " (default TEXT)",
  // default
  "text"
};

const Opt<FileSet> graphs{
  // name
  "-g,--graphs",
  // description
  "list of root relative directories where placing graphs"
};

const Opt<std::string> private_prefix{
  // name
  "-p,--private",
  // descritption
  "prefix used for private (not to be documented) IDs (variable, function, module"
  " or whatever)",
  // defaults
  "__"
};

const Version version{
  // name
  "-v,--version",
  // description
  "orthodocs version " ODOX_VERSION_STR " commit " ODOX_VERSION_COMMIT
};

const Map<Verbosity> verbosity{
  // name
  "-V,--verbosity",
  // description
  "set spdlog verbosity",
  // map
  {
    {"trace",     Verbosity::trace    },
    {"debug",     Verbosity::debug    },
    {"info",      Verbosity::info     },
    {"warn",      Verbosity::warn     },
    {"error",     Verbosity::err      },
    {"critical",  Verbosity::critical },
    {"off",       Verbosity::off      }
  },
  // defaults
  Verbosity::err
};

const Flag orthodox{
  // name
  "-o,--orthodox,!--unorthodox",
  // description
  "comments for parameters are preceding their formal definition",
  // defaults
  true
};

const Opt<std::string> language{
  // name
  "<no name yet>",
  // description
  "<no description either>",
  // defaults
  "scad"
};

const Opt<std::string> writer{
  // name
  "<no name yet>",
  // description
  "<no description either>",
  // defaults
  "markdown"
};

}
