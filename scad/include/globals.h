/*
 * globals switch
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
 * when true, toc generation on the document root is enabled
 */
extern bool toc;

extern std::string prefix;

}
