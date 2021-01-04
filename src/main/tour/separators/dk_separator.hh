#ifndef DK_SEPARATOR_HH
#define DK_SEPARATOR_HH

#include "graph/graph.hh"
#include "graph/edge_map.hh"
#include "graph/vertex_map.hh"

class DKSeparator
{
private:
  const Graph& graph;

  double getViolation(const EdgeFunc<double>& weights,
                      const std::vector<Vertex>& vertices);

public:
  DKSeparator(const Graph& graph)
    : graph(graph)
  {}

  VertexMap<num> computeIndices(const std::vector<Vertex>& vertices);

  EdgeMap<num> computeFactors(const std::vector<Vertex>& vertices,
                              const VertexFunc<num>& indices);

  std::vector<std::vector<Vertex>> separate(const EdgeFunc<double>& weights,
                                            int maxNumCycles = -1);

};


#endif /* DK_SEPARATOR_HH */
