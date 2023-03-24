/*
 * mermaid graph definition file
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "markdown/graph.h"
#include <commons/error_info.h>

using namespace std;

namespace graph {

ostream &Connection::write(ostream &os,Node::Map &node_map) {
  os << "    ";
  source.write(os,node_map);
  os << " --o|" << (type==Type::inc ? "include" : "use") << "| ";
  destination.write(os,node_map);
  os << endl;
  return os;
}

ostream &Node::write(ostream &os,Node::Map &node_map) {
  if (!defined) {
    label = (++(*labeller)).string();
    os << label << '[' << name() << "]";
    defined = true;
    if (auto [i,success] = node_map.try_emplace(name(),*this); !success)
      throw std::domain_error(ERR_INFO+"Duplicate key «"+i->first+"» in same node map");
  } else
    os << label;
  return os;
}

}
