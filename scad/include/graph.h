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
  using Map = std::map<std::string,Node>;
  Node(const std::filesystem::path &abstract_path, IncLabel &label) : path(abstract_path), labeller(&label) {}

  std::ostream &write(std::ostream &os,Map &nodemap);
  
  bool        defined = false;
  IncLabel    *labeller;
  std::string label;
  std::filesystem::path    path;
  std::string name() {return path.parent_path()/path.stem();}
};

struct Connection {
  enum Type {inc,use};


  Connection(Node &source, Type type, Node &destination) : source(source),type(type),destination(destination) {}
  std::ostream &write(std::ostream &os,Node::Map &nodemap);

  Node &source,&destination;
  Type type;
};

}
