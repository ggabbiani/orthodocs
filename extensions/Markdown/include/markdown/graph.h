#pragma once
/*
 * mermaid graph declaration file
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
 * along with ODOX.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "orthodocs/utils.h"

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
