#ifndef DIJKSTRA_RANK_HH
#define DIJKSTRA_RANK_HH

#include <vector>

#include "graph/graph.hh"

#include "graph/edge_map.hh"
#include "graph/vertex_map.hh"

std::vector<Vertex> nearestVertices(const Graph& graph,
                                    Vertex source,
                                    const EdgeFunc<num>& costs,
                                    int size = -1);

VertexMap<idx> dijkstraRanks(const Graph& graph,
                             Vertex source,
                             const EdgeFunc<num>& costs);

#endif /* DIJKSTRA_RANK_HH */
