#ifndef MAX_FLOW_HH
#define MAX_FLOW_HH

#include "graph/graph.hh"
#include "graph/edge_map.hh"
#include "graph/vertex_set.hh"

struct MaxFlowResult
{
  MaxFlowResult(const Graph& graph)
    : flow(graph, 0),
      cut(graph),
      value(0)
  {

  }

  EdgeMap<double> flow;
  VertexSet cut;
  double value;
};

struct MinCutResult
{
  MinCutResult(const Graph& graph)
    : cut(graph),
      value(inf)
  {

  }

  MinCutResult(const VertexSet& cut,
               double value)
    : cut(cut),
      value(value)
  {
  }

  VertexSet cut;
  double value;
};

MaxFlowResult computeMaxFlow(const Graph& graph,
                             const Vertex& source,
                             const Vertex& target,
                             const EdgeFunc<double>& capacities);

MinCutResult computeMinCut(const Graph& graph,
                           const EdgeFunc<double>& capacities);

MinCutResult computeMinCut(const Graph& graph,
                           const EdgeFunc<double>& capacities,
                           const Vertex& source);

#endif /* MAX_FLOW_HH */
