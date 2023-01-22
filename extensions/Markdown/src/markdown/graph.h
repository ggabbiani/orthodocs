#pragma once
/*
 * mermaid graph declaration file
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later */

#include <utils.h>

#include <filesystem>
#include <map>
#include <string>

namespace graph {

struct Node {
  using Map = std::map<std::string,Node,std::less<>>;
  Node(const std::filesystem::path &abstract_path, IncLabel &label) : labeller(&label),path(abstract_path) {}

  std::ostream &write(std::ostream &os,Map &nodemap);
  std::string name() const {return (path.parent_path()/path.stem()).string();}

  bool                   defined = false;
  IncLabel              *labeller;
  std::string            label;
  std::filesystem::path  path;
};

struct Connection {
  enum class Type {inc,use};

  Connection(Node &source, Type type, Node &destination) : source(source),destination(destination),type(type) {}
  std::ostream &write(std::ostream &os,Node::Map &nodemap);

  Node  &source;
  Node  &destination;
  Type   type;
};

}
