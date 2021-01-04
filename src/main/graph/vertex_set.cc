#include "vertex_set.hh"

#include "graph.hh"

VertexSet::VertexSet(const Graph& graph)
  : values(graph.getVertices().size(), false)
{}

bool VertexSet::contains(const Vertex& vertex) const
{
  return values[vertex.getIndex()];
}

void VertexSet::insert(const Vertex& vertex)
{
  values[vertex.getIndex()] = true;
}

void VertexSet::remove(const Vertex& vertex)
{
  values[vertex.getIndex()] = false;
}

bool VertexSet::contains(const Edge& edge)
{
  return contains(edge.getSource()) && contains(edge.getTarget());
}
