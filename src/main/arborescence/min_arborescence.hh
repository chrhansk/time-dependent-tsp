#ifndef MIN_ARBORESCENCE_HH
#define MIN_ARBORESCENCE_HH

#include "graph/graph.hh"
#include "graph/edge_map.hh"
#include "graph/edge_set.hh"

EdgeSet minArborescence(const Graph& graph,
                        const Vertex& root,
                        const EdgeFunc<num>& weights);

#endif /* MIN_ARBORESCENCE_HH */
