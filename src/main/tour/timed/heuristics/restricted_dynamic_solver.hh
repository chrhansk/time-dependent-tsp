#ifndef RESTRICTED_DYNAMIC_SOLVER_HH
#define RESTRICTED_DYNAMIC_SOLVER_HH

#include "graph/graph.hh"

#include "timed/timed_edge_func.hh"
#include "timed/timed_vertex_func.hh"
#include "timed/timed_router.hh"

#include "tour/tour.hh"

/**
 * This solver solves the dynamic programming formulation
 * for the TSP but disregards seemingly suboptimal tours.
 *
 * See: "A restricted dynamic programming heuristic algorithm
 *       for the time dependent traveling salesman problem"
 **/
class RestrictedDynamicSolver
{
private:
  const Graph& graph;
  const std::vector<Vertex>& vertices;
  idx maxSize;

public:
  RestrictedDynamicSolver(const Graph& graph,
                          const std::vector<Vertex>& vertices,
                          idx maxSize = 10);

  Tour findTour(TimedDistanceFunc& timedDistances);
};

#endif /* RESTRICTED_DYNAMIC_SOLVER_HH */
