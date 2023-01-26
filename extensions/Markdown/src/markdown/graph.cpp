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

ostream &Connection::write(ostream &os,Node::Map &nodemap) {
  os << "    ";
  source.write(os,nodemap);
  os << " --o|" << (type==Type::inc ? "include" : "use") << "| ";
  destination.write(os,nodemap);
  os << endl;
  return os;
}

ostream &Node::write(ostream &os,Node::Map &nodemap) {
  if (!defined) {
    label = (++(*labeller)).string();
    os << label << '[' << name() << "]";
    defined = true;
    if (auto [i,success] = nodemap.try_emplace(name(),*this); !success)
      throw std::domain_error(ERR_INFO+"Duplicate key «"+i->first+"» in same nodemap");
  } else
    os << label;
  return os;
}

}
