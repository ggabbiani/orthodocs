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
 * along with ODOX.  If not, see <http: //www.gnu.org/licenses/>.
 */

#include "utils.h"

#include <filesystem>
#include <string>

namespace option {

/**
 * document root in canonical form
 */
extern std::filesystem::path droot;

/**
 * source root in canonical form
 */
extern std::filesystem::path sroot;

/**
 * when true, adminitions emoji are enabled for the annotation.
 */
extern bool admonitions;
/**
 * when true, toc generation in document root is enabled
 */
extern bool toc;

extern std::string prefix;

/**
 * how package dependecies are documented (default "text")
 */
extern std::string pkg_deps;
/**
 * set of directory for partial dependencies graphs
 */
extern FileSet graphs;

/**
 * directories or files in any combination: paths can be passed either as relative to «Source root» or absolute.
 */
extern FileSet   sources;

/**
 * defines the language processor to be used. 
 * For now only "scad" is supported.
 */
extern std::string language;

/**
 * defines the language processor to be used. 
 * For now only "markdown" is supported.
 */
extern std::string writer;
}
