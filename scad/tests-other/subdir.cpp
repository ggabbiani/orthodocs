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

#include <cassert>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

using namespace std;

/**
 * check if sub is a subdirectory of root.
 * 
 * **NOTE:** both the arguments must be in **CANONICAL** form.
 */
bool is_sub_of(const fs::path &sub, const fs::path &root) {
  if (root.is_relative()) 
    throw runtime_error("root path not in (canonical) absolute format");
  if (sub.is_relative()) 
    throw runtime_error("sub path not in (canonical) absolute format");
  auto r_first  = root.begin();
  auto s_first  = sub.begin();
  auto r_last   = root.end();
  auto s_last   = sub.end();

  auto r_it = r_first;
  auto s_it = s_first;
  while (r_it!=r_last && s_it!=s_last && *r_it==*s_it) {
    cout << *r_it << endl;
    ++r_it;
    ++s_it;
  }
  return r_it==r_last;
}

int main(int argc, char const *argv[]) {
  try {
    assert(is_sub_of("/home/giampa/projects/OFL/foundation","/home/giampa/projects/OFL")==true);
    assert(is_sub_of("/home/giampa/projects/OFL","/home/giampa/projects/OFL/foundation")==false);
    assert(is_sub_of("/home/giampa/projects/OFL/foundation/defs.scad","/home/giampa/projects/OFL")==true);
    assert(is_sub_of("/home/giampa/projects/OFL/foundation/not-existing","/home/giampa/projects/OFL")==true);
  } catch(const exception &error) {
    cerr << error.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
