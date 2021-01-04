#ifndef TWO_OPT_SOLVER_HH
#define TWO_OPT_SOLVER_HH

#include "graph/graph.hh"
#include "graph/edge_map.hh"

#include "tour/tour.hh"

class TwoOptSolver
{
private:
  const Graph& graph;
  const std::vector<Vertex> vertices;

  typedef std::iterator_traits<std::vector<Vertex>::iterator>::difference_type DifferenceType;

  DifferenceType improve(Tour& tour, TourEvaluator<>& evaluator, DifferenceType difference);

public:
  TwoOptSolver(const Graph& graph,
               const std::vector<Vertex>& vertices);

  Tour findTour(TourEvaluator<>& evaluator, const Tour& initialTour);
};

#endif /* TWO_OPT_SOLVER_HH */
