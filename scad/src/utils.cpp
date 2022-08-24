/*
 * Empty file description
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

#include "utils.h"

#include <cassert>
#include <iostream>

using namespace std;

namespace fs=std::filesystem;

//! return a vector containing all the file paths with «extension»
void lookup(
  //! list of source directories/files
  const vector<fs::path> &sources,
  //! extension to filter out
  const char *extension, 
  //! list of source files matching «extension»
  vector<fs::path> *result
) {
  for(auto &path: sources) {
    if (fs::is_regular_file(path)) {
      if (!extension || path.extension()==extension)
        result->push_back(path);
    } else if (fs::is_directory(path)) {
      for (auto &entry: fs::directory_iterator{path}) {
        auto path = entry.path();
        if (fs::is_regular_file(path)) {
          if (!extension || path.extension()==extension)
            result->push_back(path);
        } else if (fs::is_directory(path)) {
          lookup(vector<fs::path>{path},extension,result);
        }
      }
    }
  }
}

void print_exception(const exception& e, int level) {
  cerr << string(level, ' ') << "exception: " << e.what() << '\n';
  try {
    rethrow_if_nested(e);
  } catch(const exception &nestedException) {
    print_exception(nestedException, level+1);
  } catch(...) {}
}
