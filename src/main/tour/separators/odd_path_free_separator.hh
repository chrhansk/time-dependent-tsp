#ifndef ODD_PATH_FREE_SEPARATOR_HH
#define ODD_PATH_FREE_SEPARATOR_HH

#include "timed/time_expanded_graph.hh"

class OddPathFreeSet
{
private:
  std::vector<TimedEdge> edges;
  std::vector<Vertex> originalVertices;

public:
  OddPathFreeSet(const std::vector<TimedEdge>& edges,
                 const std::vector<Vertex>& originalVertices)
    : edges(edges),
      originalVertices(originalVertices)
  {}

  const std::vector<TimedEdge>& getEdges() const
  {
    return edges;
  }

  const std::vector<Vertex>& getOriginalVertices() const
  {
    return originalVertices;
  }
};

class OddPathFreeSeparator
{
private:
  const TimeExpandedGraph& graph;
  const Graph& originalGraph;
  Vertex originalSource;

  double getWeight(const std::vector<Vertex> originalVertices,
                   const EdgeFunc<double>& originalWeights);

public:
  OddPathFreeSeparator(const TimeExpandedGraph& graph,
                       const Vertex& originalSource)
    : graph(graph),
      originalGraph(graph.underlyingGraph()),
      originalSource(originalSource)
  {}

  std::vector<OddPathFreeSet> separate(const EdgeFunc<double>& weights,
                                       int maxNumSets = -1);

};


#endif /* ODD_PATH_FREE_SEPARATOR_HH */
