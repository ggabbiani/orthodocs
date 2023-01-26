/*
 * Annotations utils
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "annotations.h"

#include <commons/utils.h>

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
  for(const auto &[search_s, insert_s]: admonitions) {
    size_t pos = 0;
    while ((pos = nocase::rfind(anno,search_s,pos)) != std::string::npos) {
        anno.insert(anno.length()-pos-search_s.length(),insert_s);
        pos += insert_s.length();
    }
  }

}
