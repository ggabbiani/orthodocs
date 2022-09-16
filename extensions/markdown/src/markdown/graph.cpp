/*
 * mermaid graph definition file
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

#include "markdown/graph.h"

// #include <iostream>

using namespace std;

namespace graph {

ostream &Connection::write(ostream &os,Node::Map &nodemap) {
  os << "    ";
  source.write(os,nodemap);
  os << " --o|" << (type==inc ? "include" : "use") << "| ";
  destination.write(os,nodemap);
  os << endl;
  return os;
}

ostream &Node::write(ostream &os,Node::Map &nodemap) {
  if (!defined) {
    label = (++(*labeller)).string();
    os << label << '[' << name() << "]";
    defined = true;
    nodemap.emplace(name(),*this);
  } else
    os << label;
  return os;
}

}
