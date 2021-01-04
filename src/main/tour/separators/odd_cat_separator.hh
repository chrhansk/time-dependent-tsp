#ifndef ODD_CAT_SEPARATOR_HH
#define ODD_CAT_SEPARATOR_HH

#include "graph/graph.hh"
#include "graph/edge_map.hh"
#include "graph/vertex_map.hh"

class OddCATSeparator
{
private:
  const Graph& graph;

  bool areIncompatible(const Edge& first, const Edge& second) const;

  void createIncompatibilityGraph(const EdgeFunc<double>& weights,
                                  Graph& incompatGraph,
                                  EdgeMap<double>& incompatWeights,
                                  VertexMap<Edge>& originalEdges) const;

  bool validCycle(const std::vector<Edge>& cycle) const;

  double getViolation(const EdgeFunc<double>& weights,
                      const std::vector<Edge>& cycle) const;

public:
  OddCATSeparator(const Graph& graph)
    : graph(graph)
  {}

  std::vector<std::vector<Edge>> separate(const EdgeFunc<double>& weights,
                                          int maxNumCATs = -1);
};


#endif /* ODD_CAT_SEPARATOR_HH */
