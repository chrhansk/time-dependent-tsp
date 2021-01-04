#include "edge_set.hh"

#include "graph.hh"

EdgeSet::EdgeSet(const Graph& graph)
  : values(graph.getEdges().size(), false)
{}

bool EdgeSet::contains(const Edge& edge) const
{
  return values[edge.getIndex()];
}

void EdgeSet::insert(const Edge& edge)
{
  values[edge.getIndex()] = true;
}

void EdgeSet::remove(const Edge& edge)
{
  values[edge.getIndex()] = false;
}
