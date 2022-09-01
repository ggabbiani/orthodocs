#pragma once

#include "document.h"
#include "utils.h"

#include <ostream>
// #include <sstream>
#include <string>
#include <map>
#include <memory>
#include <set>

namespace graph {

struct Node {
  Node(const doc::Package *pkg,IncLabel &label) : package(pkg),labeller(&label) {}

  std::ostream &write(std::ostream &os);
  
  const std::string &name() const {return package->name;}

  bool                defined = false;
  const doc::Package  *package;
  IncLabel            *labeller;
  std::string         label;
};

struct Connection {
  enum Type {inc,use};

  Connection(Node &source, Type type, Node &destination) : source(source),type(type),destination(destination) {}
  std::ostream &write(std::ostream &os);

  Node &source,&destination;
  Type type;
};

}
