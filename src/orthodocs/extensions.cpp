/*
 * extension management implementation
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

#include "orthodocs/config.h"
#include "orthodocs/extensions.h"
#include "orthodocs/globals.h"
#include "singleton.h"

using namespace std;

namespace language {

Extension *Extension::factory() {
  auto &registry  = Singleton<Map>::instance();
  auto i          = registry.find(option::language);
  if (i!=registry.end())
    return i->second;
  else
    throw runtime_error("No language extension found for id '"+option::language+'\'');
}

} // namespace language

namespace writer {

Extension *Extension::factory() {
  auto &registry  = Singleton<Map>::instance();
  auto i          = registry.find(option::writer);
  if (i!=registry.end())
    return i->second;
  else
    throw runtime_error("No language extension found for id '"+option::writer+'\'');
}

} // namespace writer
