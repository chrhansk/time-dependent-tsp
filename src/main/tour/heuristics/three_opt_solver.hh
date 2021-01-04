#ifndef THREE_OPT_SOLVER_HH
#define THREE_OPT_SOLVER_HH

#include "graph/graph.hh"

#include "tour/tour.hh"

class ThreeOptSolver
{
private:
  const Graph& graph;
  const std::vector<Vertex> vertices;

  void improve(TourEvaluator<>& evaluator,
               Tour& tour);

public:
  ThreeOptSolver(const Graph& graph,
                 const std::vector<Vertex>& vertices);

  Tour findTour(TourEvaluator<>& evaluator,
                const Tour& initialTour);
};

#endif /* THREE_OPT_SOLVER_HH */
