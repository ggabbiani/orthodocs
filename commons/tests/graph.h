#pragma once

/*
 * Generic graph class.
 *
 * This file contains modified code based on the original graph.h source code
 * part of the 'varia' project written by Martijn Saelens and distributed under
 * the terms of the MIT License. The original code can be found at:
 * https://gitlab.com/MartenBE/varia/-/blob/master/include/graph/graph.hpp
 *
 * The modifications made to this file are distributed under the terms of the GPL
 * License as published by the Free Software Foundation, version 3 or later.
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <debug/trace.h>

#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <vector>

template <
  class VDATA, class EDATA,
  typename VID = typename VDATA::Id, typename EID = typename EDATA::Id
> class Graph {
private:
  class Vertex;
  using This      = Graph<VDATA, EDATA, VID, EID>;
  using VertexMap = std::map<VID, Vertex>;
  using EdgeMap   = std::map<VID, EDATA>;

public:
  friend void swap(Graph& lhs, Graph& rhs) {
    std::swap(lhs._vertices, rhs._vertices);
  }

  /*
   * add a vertex
   */
  VID add(const VDATA &data) {
    TR_FUNC;
    VID id{data};
    if (auto [i, success] = _vertices.try_emplace(id, data); !success)
      throw std::runtime_error("Vertex id '" + id.string() + "' already present");
    return id;
  }

  const VDATA& vertex_data(const VID& id) const {
    assert(contains(id));
    return _vertices[id].data();
  }

  bool contains(const VID& id) const {
    return _vertices.contains(id);
  }

  void remove_vertex(const VID& id) {
    _vertices.erase(id);
  }

  /*
   * return the number ov vertices
   */
  int size() const {
    return _vertices.size();
  }

  void clear() {
    *this = This{};
  }

  /*
   * add an edge by «from» and «to»  VID
   */
  EID add(const VID& from, const VID& to, const EDATA& data) {
    TR_FUNC;
    EID id{from,to};

    if (!contains(to))
      throw std::runtime_error("Vertex id '"+to.string()+"' not present");
    try {
      if (auto [i,success] = _vertices.at(from).list.try_emplace(to,data); !success)
        throw std::runtime_error("Edge id '" + id.string() + "' already present");
    } catch(const std::out_of_range&) {
      throw_with_nested(std::runtime_error("Vertex id '"+from.string()+"' not present"));
    }
    return id;
  }

  /*
   * add an edge by EID
   */
  EID add(const EID &id, const EDATA& data) {
    TR_FUNC;
    if (!contains(id.to))
      throw std::runtime_error("Vertex id '"+id.to.string()+"' not present");
    try {
      if (auto [i,success] = _vertices.at(id.from).list.try_emplace(id.to,data); !success)
        throw std::runtime_error("Edge id '" + id.string() + "' already present");
    } catch(const std::out_of_range&) {
      throw_with_nested(std::runtime_error("Vertex id '"+id.from.string()+"' not present"));
    }
    return id;
  }

  std::pair<EID, EID>
  add_undirected_edge(const VID& from, const VID& to, const EDATA& data) {
    EID a = add(from, to, data);
    EID b = add(to, from, data);

    return {a, b};
  }

  const EDATA& edge_data(const VID& from, const VID& to) const {
    assert(is_edge_present(from, to));

    return _vertices[from].list[to].data;
  }

  bool is_edge_present(const VID& from, const VID& to) const {
    return _vertices[from].list.contains(to);
  }

  void remove_edge(const VID& from, const VID& to) {
    _vertices[from].list.erase(to);
  }

  size_t amount_edges() const {
    size_t size = 0;
    for_each(_vertices.begin(), _vertices.end(), [&size](const std::pair<VID, Vertex>& value) {
      size += value.second.list.size();
    });
    return size;
  }

private:
  class Vertex {
    friend Graph;
  public:
    explicit Vertex(const VDATA& data) : data{data} {}

  private:
    EdgeMap list;
    VDATA   data;
  };

  /*
   * adjacency list: every element is a vertex containing a list of the vertex
   * to which is connected to
   */
  VertexMap _vertices;

};
