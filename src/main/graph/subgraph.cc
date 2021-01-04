#include "subgraph.hh"

SubGraph::SubGraph(const Graph& parent,
                   const std::vector<Vertex>& vertices)
  : parent(parent),
    originalVertices(graph, Vertex())
{
  std::vector<Vertex> myvertices(vertices.size());

  for(const Vertex& vertex : vertices)
  {
    originalVertices.addVertex(graph.addVertex(), vertex);
  }
}

Vertex SubGraph::originalVertex(Vertex vertex) const
{
  return originalVertices(vertex);
}

Edge SubGraph::addEdge(Vertex source, Vertex target)
{
  return graph.addEdge(source, target);
}
