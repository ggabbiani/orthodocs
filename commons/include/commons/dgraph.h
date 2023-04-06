#pragma once

/*
 * Document graph header
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <commons/document.h>
#include <commons/graph.h>

namespace doc {

class Graph {
public:
  using VertexData    = Document::Vertex;
  using EdgeData      = Document::Edge;
  using AdjacencyList = graph<VertexData,EdgeData>;
  using Id            = std::ptrdiff_t;
  using IdSet         = std::set<Id>;

  Graph() = default;
  Id add(const Document *document);
  IdSet vertices() const;
  const VertexData &vertex(Id i) const;
private:
  AdjacencyList _alist;
};

}