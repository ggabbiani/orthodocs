#include "graph.h"
#include "utils.h"
#include <iostream>

using namespace std;

namespace graph {

ostream &Connection::write(ostream &os) {
  os << "    ";
  source.write(os);
  os << " -->|" << (type==inc ? "include" : "use") << "| ";
  destination.write(os);
  os << endl;
  return os;
}

ostream &Node::write(ostream &os) {
  if (!defined) {
    label = (++(*labeller)).string();
    os << label << '[' << name() << "]";
    defined = true;
  } else
    os << label;
  return os;
}

}
