#ifndef EXACT_TIMED_SOLVER_HH
#define EXACT_TIMED_SOLVER_HH

#include "graph/graph.hh"

#include "timed/timed_vertex_func.hh"

#include "tour/tour.hh"

class ExactTimedSolver
{
private:
  const Graph& graph;
  const std::vector<Vertex> vertices;

public:
  ExactTimedSolver(const Graph& graph,
                   const std::vector<Vertex>& vertices);

  Tour findTour(TimedDistanceFunc& distances,
                const Tour& initialTour);
};

#endif /* EXACT_TIMED_SOLVER_HH */
