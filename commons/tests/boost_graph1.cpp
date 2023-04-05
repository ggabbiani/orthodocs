/*
 * boost libraries graph test: this is propaedeutic to synthetic graph (a
 * forest actually) representation
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each

using namespace boost;
using namespace std;

struct Package {
  string  name;
};

struct Dependency {
  enum Type {
    typ_undefined,
    typ_include,
    typ_use
  };
  explicit Dependency(Type type=typ_undefined) : type{type} {}
  const Type type;
};

using Graph         = adjacency_list<vecS, vecS, directedS, Package, Dependency>;
using Edge          = std::pair<unsigned,unsigned>;
using Vertex        = graph_traits<Graph>::vertex_descriptor;
using IndexMap      = property_map<Graph, vertex_index_t>::type;
using vertex_iter   = graph_traits<Graph>::vertex_iterator;
using edge_iterator = graph_traits<Graph>::edge_iterator;

int main(int,char*[]) {
  // Make convenient labels for the vertices
  enum { A, B, C, D, E, N };
  const int num_vertices = N;
  const char* name = "ABCDE";

  // writing out the edges in the graph
  Edge edge_array[] = {
    Edge(A,B),
    Edge(A,D),
    Edge(C,A),
    Edge(D,C),
    Edge(C,E),
    Edge(B,D),
    Edge(B,D),
    Edge(D,E)
  };
  const int num_edges = sizeof(edge_array)/sizeof(edge_array[0]);

  // declare a graph object
  Graph g;
  // Graph g(num_vertices);
  // Graph g(edge_array, edge_array + sizeof(edge_array) / sizeof(Edge), num_vertices);

  // add the edges to the graph object
  for(auto i = 0; i<num_edges; ++i) {
    auto [edge,boh] = add_edge(edge_array[i].first, edge_array[i].second, g);
    cout << boolalpha << "r{" << edge << ',' << boh << '}' << endl;
  }

  // accessing the Vertex Set

  // get the property map for vertex indices
  IndexMap index = get(vertex_index, g);

  cout << "vertices(g) = ";
  for (auto [iterator, end] = vertices(g); iterator!=end; ++iterator) {
    Vertex descriptor = *iterator;
    cout << index[descriptor] <<  " ";
  }
  cout << endl;

  cout << "edges(g) = ";
  for (auto [iterator, end] = edges(g); iterator!=end; ++iterator)
    cout  << "(" << index[source(*iterator, g)]
          << "," << index[target(*iterator, g)] << ") ";
  cout << endl;
  // ...

  return EXIT_SUCCESS;
}
