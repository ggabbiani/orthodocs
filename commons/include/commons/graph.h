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

#include <map>
#include <nlohmann/json.hpp>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <vector>

template <class V, class E>
class graph {
 private:
  class vertex;
  class edge;

 public:
  using Id = std::string;

  friend void swap(graph& lhs, graph& rhs) {
    std::swap(lhs.m_vertices, rhs.m_vertices);
    std::swap(lhs.m_freed_vertex_ids, rhs.m_freed_vertex_ids);

    std::swap(lhs.m_edges, rhs.m_edges);
    std::swap(lhs.m_freed_edge_ids, rhs.m_freed_edge_ids);
  }

  // Functions ///////////////////////////////////////////////////////////////
  std::ptrdiff_t add_vertex(const V& data) {
    std::ptrdiff_t new_vertex_id = 0;
    if (!m_freed_vertex_ids.empty()) {
      new_vertex_id = *m_freed_vertex_ids.begin();
      m_freed_vertex_ids.erase(new_vertex_id);
      m_vertices[new_vertex_id] = vertex{new_vertex_id, data};
    } else {
      new_vertex_id = std::ssize(m_vertices);
      m_vertices.emplace_back(new_vertex_id, data);
    }

    return new_vertex_id;
  }

  const V& vertex_data(std::ptrdiff_t vertex_id) const {
    assert(is_vertex_present(vertex_id));

    return m_vertices[vertex_id].data();
  }

  bool is_vertex_present(std::ptrdiff_t vertex_id) const {
    return is_id_in_use(vertex_id, (std::ssize(m_vertices) - 1), m_freed_vertex_ids);
  }

  void remove_vertex(std::ptrdiff_t vertex_id) {
    m_freed_vertex_ids.insert(vertex_id);

    for (const auto& edge : m_edges) {
      if ((edge.source_vertex_id() == vertex_id) || (edge.target_vertex_id() == vertex_id)) {
        m_vertices[edge.source_vertex_id()].remove_edge(edge.id());
        m_freed_edge_ids.insert(edge.id());
      }
    }
  }

  int amount_vertices() const {
    return (std::ssize(m_vertices) - std::ssize(m_freed_vertex_ids));
  }

  void clear() {
    *this = graph<V, E>{};
  }

  std::ptrdiff_t add_edge(std::ptrdiff_t from_vertex_id, std::ptrdiff_t to_vertex_id, const E& data) {
    assert(is_vertex_present(from_vertex_id));
    assert(is_vertex_present(to_vertex_id));

    std::ptrdiff_t new_edge_id;

    if (!m_freed_edge_ids.empty()) {
      new_edge_id = *m_freed_edge_ids.begin();
      m_freed_edge_ids.erase(new_edge_id);
      m_edges[new_edge_id] = edge{new_edge_id, from_vertex_id, to_vertex_id, data};
    } else {
      new_edge_id = std::ssize(m_edges);
      m_edges.emplace_back(new_edge_id, from_vertex_id, to_vertex_id, data);
    }

    m_vertices[from_vertex_id].add_edge(new_edge_id);

    return new_edge_id;
  }

  std::pair<std::ptrdiff_t, std::ptrdiff_t>
  add_undirected_edge(std::ptrdiff_t from_vertex_id, std::ptrdiff_t to_vertex_id, const E& data) {
    std::ptrdiff_t a = add_edge(from_vertex_id, to_vertex_id, data);
    std::ptrdiff_t b = add_edge(to_vertex_id, from_vertex_id, data);

    return {a, b};
  }

  const E& edge_data(std::ptrdiff_t edge_id) const {
    assert(is_edge_present(edge_id));

    return m_edges[edge_id].data();
  }

  std::ptrdiff_t edge_source_vertex_id(std::ptrdiff_t edge_id) const {
    return m_edges[edge_id].source_vertex_id();
  }

  std::ptrdiff_t edge_target_vertex_id(std::ptrdiff_t edge_id) const {
    return m_edges[edge_id].target_vertex_id();
  }

  bool is_edge_present(std::ptrdiff_t edge_id) const {
    return is_id_in_use(edge_id, (std::ssize(m_edges) - 1), m_freed_edge_ids);
  }

  void remove_edge(std::ptrdiff_t edge_id) {
    m_freed_edge_ids.insert(edge_id);
  }

  std::ptrdiff_t amount_edges() const {
    return (std::ssize(m_edges) - std::ssize(m_freed_edge_ids));
  }

  void clear_edges() {
    m_edges.clear();
    m_freed_edge_ids.clear();

    for (auto& v : m_vertices) {
      v.clear_edges();
    }
  }

  bool empty() const {
    return (amount_vertices() == 0);
  }

  std::set<std::ptrdiff_t> edge_ids(std::ptrdiff_t vertex_id) const {
    assert(is_vertex_present(vertex_id));

    return m_vertices[vertex_id].edge_ids();
  }

  std::set<std::ptrdiff_t> vertex_ids() const {
    std::set<std::ptrdiff_t> ids;

    for (std::ptrdiff_t i = 0; i < std::ssize(m_vertices); i++) {
      if (!m_freed_vertex_ids.contains(i)) {
        ids.insert(i);
      }
    }

    return ids;
  }

  std::set<std::ptrdiff_t> edge_ids() const {
    std::set<std::ptrdiff_t> ids;

    for (std::ptrdiff_t i = 0; i < std::ssize(m_edges); i++) {
      if (!m_freed_vertex_ids.contains(i)) {
        ids.insert(i);
      }
    }

    return ids;
  }

  void transpose() {
    for (auto& edge : m_edges) {
      m_vertices[edge.source_vertex_id()].remove_edge(edge.id());
      edge.transpose();
      m_vertices[edge.source_vertex_id()].add_edge(edge.id());
    }
  }

  nlohmann::json to_json() const {
    nlohmann::json graph_json;

    graph_json["graph"]["nodes"] = nlohmann::json::array();

    for (const auto& vertex : m_vertices) {
      if (!m_freed_vertex_ids.contains(vertex.id())) {
        nlohmann::json vertex_json;

        vertex_json["id"] = vertex.id();
        vertex_json["edge_ids"] = vertex.edge_ids();

        std::stringstream data;
        data << vertex.data();
        vertex_json["data"] = data.str();
        data.clear();

        graph_json["graph"]["nodes"].push_back(vertex_json);
      }
    }

    graph_json["graph"]["edges"] = nlohmann::json::array();

    for (const auto& edge : m_edges) {
      if (!m_freed_edge_ids.contains(edge.id())) {
        nlohmann::json edge_json;

        edge_json["id"] = edge.id();
        edge_json["source"] = edge.source_vertex_id();
        edge_json["target"] = edge.target_vertex_id();

        std::stringstream data;
        data << edge.data();
        edge_json["data"] = data.str();
        data.clear();

        graph_json["graph"]["edges"].push_back(edge_json);
      }
    }

    return graph_json;
  }

  std::string to_dot() const {
    std::stringstream out;
    out << "digraph {\n";

    for (const auto& v : m_vertices) {
      if (!m_freed_vertex_ids.contains(v.id())) {
        out << "\t" << v.id() << " [label=\"(" << v.id() << ") " << v.data() << "\"]\n";
      }
    }

    for (const auto& e : m_edges) {
      if (!m_freed_edge_ids.contains(e.id())) {
        out << "\t" << e.source_vertex_id() << " -> " << e.target_vertex_id() << " [label=\"(" << e.id() << ") "
            << e.data() << "\"]\n";
      }
    }

    out << "}";

    return out.str();
  }

  // Operators ///////////////////////////////////////////////////////////////

  friend std::ostream& operator<<(std::ostream& os, const graph& object) {
    os << object.to_json().dump(4);

    return os;
  }

  // Nested classes //////////////////////////////////////////////////////////

 private:
  class edge {
   public:
    edge(std::ptrdiff_t id, std::ptrdiff_t source_vertex_id, std::ptrdiff_t target_vertex_id, const E& data)
        : m_id{id}, m_source_vertex_id{source_vertex_id}, m_target_vertex_id{target_vertex_id}, m_data{data} {
    }

    std::ptrdiff_t id() const {
      return m_id;
    }

    std::ptrdiff_t source_vertex_id() const {
      return m_source_vertex_id;
    }

    std::ptrdiff_t target_vertex_id() const {
      return m_target_vertex_id;
    }

    const E& data() const {
      return m_data;
    }

    void transpose() {
      std::swap(m_source_vertex_id, m_target_vertex_id);
    }

   private:
    std::ptrdiff_t m_id;
    std::ptrdiff_t m_source_vertex_id;
    std::ptrdiff_t m_target_vertex_id;
    E m_data;
  };

  class vertex {
   public:
    vertex(std::ptrdiff_t id, const V& data) : m_id{id}, m_data{data} {
    }

    std::ptrdiff_t id() const {
      return m_id;
    }

    void add_edge(std::ptrdiff_t edge_id) {
      m_edge_ids.insert(edge_id);
    }

    void remove_edge(std::ptrdiff_t edge_id) {
      m_edge_ids.erase(edge_id);
    }

    void clear_edges() {
      m_edge_ids.clear();
    }

    std::set<std::ptrdiff_t> edge_ids() const {
      return m_edge_ids;
    }

    const V& data() const {
      return m_data;
    }

   private:
    std::ptrdiff_t m_id;
    std::set<std::ptrdiff_t> m_edge_ids;
    V m_data;
  };

  bool is_id_in_use(std::ptrdiff_t id, std::ptrdiff_t max_id_in_use, std::set<std::ptrdiff_t> freed_ids) const {
    return ((id >= 0) && (id <= max_id_in_use) && !(freed_ids.contains(id)));
  }

  std::vector<vertex> m_vertices;
  std::set<std::ptrdiff_t> m_freed_vertex_ids;

  std::vector<edge> m_edges;
  std::set<std::ptrdiff_t> m_freed_edge_ids;

  std::map<std::string, vertex> _vertices;
};
