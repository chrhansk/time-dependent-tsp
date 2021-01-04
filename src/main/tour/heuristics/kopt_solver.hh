#ifndef KOPT_SOLVER_HH
#define KOPT_SOLVER_HH

#include <utility>

#include "graph/graph.hh"

#include "tour/tour.hh"

class KOptSolver
{
public:
  typedef std::iterator_traits<std::vector<Vertex>::iterator>::difference_type DifferenceType;

private:
  const Graph& graph;
  const std::vector<Vertex> vertices;
  const idx k;

  DifferenceType improve(TourEvaluator<>& evaluator,
                         Tour& tour,
                         DifferenceType initialDistance);

public:
  KOptSolver(const Graph& graph,
             const std::vector<Vertex>& vertices,
             const idx k);

  Tour findTour(TourEvaluator<>& evaluator,
                const Tour& initialTour);
};

#endif /* KOPT_SOLVER_HH */
