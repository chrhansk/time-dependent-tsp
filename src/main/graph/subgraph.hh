#ifndef SUBGRAPH_HH
#define SUBGRAPH_HH

#include "graph.hh"
#include "vertex_map.hh"

#include "edge_map.hh"

class SubGraph
{
private:
  const Graph& parent;
  Graph graph;
  VertexMap<Vertex> originalVertices;

public:
  SubGraph(const Graph& parent,
           const std::vector<Vertex>& vertices);

  operator const Graph&() const
  {
    return graph;
  }

  Vertex originalVertex(Vertex vertex) const;

  Edge addEdge(Vertex source, Vertex target);
};


#endif /* SUBGRAPH_HH */
