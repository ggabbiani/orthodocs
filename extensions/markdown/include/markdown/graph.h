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
