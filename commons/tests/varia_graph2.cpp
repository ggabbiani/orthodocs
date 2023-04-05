#include <commons/graph.h>

#include <iostream>

using namespace std;

struct Vertex {
  int foo;
  friend ostream & operator << (ostream &out, const Vertex &vertex) {
    return out << to_string(vertex.foo);
  }
};
struct Edge {
  string blah;
  friend ostream & operator << (ostream &out, const Edge &edge) {
    return out << edge.blah;
  }
};

using graph_t = graph<Vertex,Edge>;

int main() {

  //Instantiate a graph
  graph_t g;

  // Create two vertices in that graph
  auto u = g.add_vertex(Vertex{123});
  auto v = g.add_vertex(Vertex{456});

  // Create an edge connecting those two vertices
  auto e = g.add_edge(u, v, Edge{"Hello"});

  // Set the properties of a vertex and the edge
  // g.vertex_data(u).foo  = 42;
  // g.edge_data(e).blah = "Hello world";

  cout << "vertices = {\n";
  for (const auto &vid: g.vertex_ids()) {
    cout << g.vertex_data(vid).foo <<  "\n";
  }
  cout << '}' << endl;

  auto dot = g.to_dot();
  cout << dot << endl;

  return EXIT_SUCCESS;
}
