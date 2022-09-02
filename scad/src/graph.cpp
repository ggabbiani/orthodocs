#include "graph.h"
#include "utils.h"
#include <iostream>

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
