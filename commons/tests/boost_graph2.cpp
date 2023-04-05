#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <iostream>

using namespace boost;
using namespace std;

struct Vertex { int foo; };
struct Edge { std::string blah; };

using graph_t  = adjacency_list<listS, vecS, directedS, Vertex, Edge >;
using vertex_t = graph_traits<graph_t>::vertex_descriptor;
using edge_t   = graph_traits<graph_t>::edge_descriptor;

int main() {

  //Instantiate a graph
  graph_t g;

  // Create two vertices in that graph
  vertex_t u = boost::add_vertex(Vertex{123}, g);
  vertex_t v = boost::add_vertex(Vertex{456}, g);

  // Create an edge connecting those two vertices
  auto [e,b] = boost::add_edge(u, v, Edge{"Hello"}, g);

  // Set the properties of a vertex and the edge
  g[u].foo  = 42;
  g[e].blah = "Hello world";

  auto des = vertex(2,g);
  assert(des == 2);

  cout << "vertices = {\n";
  for (const auto &vertex: g.m_vertices) {
    cout << vertex.m_property.foo <<  "\n";
  }
  cout << '}' << endl;

  // get the property map for vertex indices
  auto index = boost::get(vertex_index,g);
  cout << "vertices(g) = {\n";
  for (auto [iterator, end] = vertices(g); iterator!=end; ++iterator) {
    auto descriptor = *iterator;
    auto foo        = g[descriptor].foo;
    cout << descriptor << "==" << index[descriptor] << ',' << foo <<  "\n";
  }
  cout << '}' << endl;



  boost::write_graphviz(std::cout, g, [&] (auto& out, auto v) {
      out << "[label=\"" << g[v].foo << "\"]";
    },
    [&] (auto& out, auto e) {
      out << "[label=\"" << g[e].blah << "\"]";
    }
  );
  std::cout << std::flush;
}
