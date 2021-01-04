#ifndef PATH_DECOMPOSITION_HH
#define PATH_DECOMPOSITION_HH

#include "time_expanded_graph.hh"
#include "timed_path.hh"

std::vector<TimedPath> pathDecomposition(const TimeExpandedGraph& graph,
                                         const EdgeFunc<double>& flow,
                                         const Vertex& source);

#endif /* PATH_DECOMPOSITION_HH */
