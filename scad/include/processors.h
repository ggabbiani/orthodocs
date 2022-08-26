/*
 * OpenSCAD source file processor
 *
 * Copyright © 2022 Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * This file is part of the 'AutoDox' (ADOX) project.
 *
 * ADOX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ADOX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ADOX.  If not, see <http: //www.gnu.org/licenses/>.
 */

#pragma once

#include "antlr4-runtime.h"

#include <filesystem>

namespace fs=std::filesystem;

namespace scad {

/**
 * Process a OpenSCAD source producing the documentation for:
 * 
 * - global variables
 * - public functions
 * - global public modules
 */
class Processor {
public:
  Processor(antlr4::BaseErrorListener &handler) : _handler(handler) {}
  /*!
   * Elaborate an OpenSCAD source and produce a Markdown document
   *
   * TODO: other kind of formatter (i.e not only Markdown)
   */
  void operator () (
    //! Source root directory
    const fs::path &sroot, 
    //! OpenSCAD source file relative to source root
    const fs::path &source, 
    //! Document root directory
    const fs::path &droot
  );
private:
  antlr4::BaseErrorListener &_handler;
};

} // namespace scad
