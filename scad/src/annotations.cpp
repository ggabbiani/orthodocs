/*
 * Annotations utils
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

#include "annotations.h"
#include "utils.h"

#include <array>
#include <cstring>
#include <regex>
#include <utility>

using namespace std;

void mk_admonitions(std::string &anno) {
  static array<pair<string,string>,3> admonitions = {
    {
      {"**note:**",     ":memo: "},
      {"**tip:**",      ":bulb: "},
      {"**warning:**",  ":warning: "}
    }
  };
  for(const auto adm: admonitions) {
    auto pos = nocase::find(anno,adm.first);
    if (pos!=string::npos)
      anno.insert(pos,adm.second);
  }
}