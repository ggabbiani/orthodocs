#include <commons/graph.h>

#include <iostream>
#include <string>

using namespace std;

struct Package {
  string  name;
};

struct Dependency {
  string  name;
};

using DependencyGraph = graph<Package,Dependency>;
using Edge            = pair<unsigned,unsigned>;

int main(int argc, const char* argv[]) {
  enum { A, B, C, D, E, N };
  string vertex_array[] {
    "A",
    "B",
    "C",
    "D",
    "E"
  };
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

  DependencyGraph g;

  for(auto i=0;i<N;++i)
    g.add_vertex({vertex_array[i]});

  for(auto i=0; i<num_edges; ++i) {
    auto eid = g.add_edge(edge_array[i].first,edge_array[i].second,{vertex_array[edge_array[i].first]+"->"+vertex_array[edge_array[i].second]});
    cout  << "r{("  << g.edge_source_vertex_id(eid)
          << ','    << g.edge_target_vertex_id(eid) << ")}" << endl;
  }

  cout << "vertices(g) = ";
  for (auto vid: g.vertex_ids())
    cout << vid <<  " ";
  cout << endl;

  cout << "edges(g) = ";
  for (auto eid: g.edge_ids())
    cout  << "("  << g.edge_source_vertex_id(eid)
          << ','  << g.edge_target_vertex_id(eid) << ") ";
  cout << endl;

}
