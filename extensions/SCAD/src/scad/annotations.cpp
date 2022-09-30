/*
 * Annotations utils
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

#include "scad/annotations.h"
#include "orthodocs/utils.h"

#include <array>
#include <cstring>
#include <regex>
#include <utility>

using namespace std;

void mk_admonitions(std::string &anno) {
  static array<pair<string,string>,10> admonitions = {
    {
      {"**caution:**",    ":caution: "  },
      {"**important:**",  ":important: "},
      {"**note:**",       ":memo: "     },
      {"**tip:**",        ":bulb: "     },
      {"**warning:**",    ":warning: "  },
      {"__caution:__",    ":caution: "  },
      {"__important:__",  ":important: "},
      {"__note:__",       ":memo: "     },
      {"__tip:__",        ":bulb: "     },
      {"__warning:__",    ":warning: "  },
    }
  };
  for(const auto adm: admonitions) {
    auto search_s = adm.first;
    auto insert_s = adm.second;
    auto pos = 0;
    while ((pos = nocase::rfind(anno,search_s,pos)) != std::string::npos) {
        anno.insert(anno.length()-pos-search_s.length(),insert_s);
        pos += insert_s.length();
    }
  }

}
