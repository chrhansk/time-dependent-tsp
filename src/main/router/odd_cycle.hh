#ifndef ODD_CYCLE_HH
#define ODD_CYCLE_HH

#include "graph/graph.hh"
#include "graph/edge_map.hh"

std::vector<std::vector<Vertex>>
findOddCycles(const Graph& incompatGraph,
              const EdgeFunc<double>& incompatWeights,
              double maxWeight);

#endif /* ODD_CYCLE_HH */
