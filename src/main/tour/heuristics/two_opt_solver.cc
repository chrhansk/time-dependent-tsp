#include "two_opt_solver.hh"

#include <iterator>

#include "log.hh"

TwoOptSolver::TwoOptSolver(const Graph& graph,
                           const std::vector<Vertex>& vertices)
  : graph(graph),
    vertices(vertices)
{

}

Tour TwoOptSolver::findTour(TourEvaluator<>& evaluator, const Tour& initialTour)
{
  Tour tour(graph, initialTour.getVertices());

  DifferenceType difference = 0;

  do
  {
    difference = improve(tour, evaluator, difference);
  }
  while(difference != -1);

  return tour;
}

TwoOptSolver::DifferenceType
TwoOptSolver::improve(Tour& tour, TourEvaluator<>& evaluator, DifferenceType difference)
{
  const num initialCost = evaluator(tour);
  Log(info) << "Initial cost: " << initialCost;

  std::vector<Vertex>::iterator first = tour.getVertices().begin();

  std::advance(first, difference);

  Tour nextTour(graph, vertices);

  for(; first != tour.getVertices().end(); ++first)
  {
    auto second = first;
    ++second;

    if(second == tour.getVertices().end())
    {
      continue;
    }

    for(; second != tour.getVertices().end(); ++second)
    {
      std::vector<Vertex>& nextVertices = nextTour.getVertices();
      nextVertices.resize(0);

      nextVertices.insert(std::end(nextVertices),
                          tour.getVertices().begin(),
                          first);

      nextVertices.insert(std::end(nextVertices),
                          std::reverse_iterator<decltype(second)>(second),
                          std::reverse_iterator<decltype(first)>(first));

      nextVertices.insert(std::end(nextVertices),
                          second,
                          tour.getVertices().end());

      assert(nextVertices.size() == vertices.size());

      num nextCost = evaluator(nextTour);

      num costDifference = nextCost - initialCost;

      if(costDifference < 0)
      {
        DifferenceType dist = std::distance(tour.getVertices().begin(), first);

        tour.getVertices() = nextVertices;

        Log(info) << "Found a 2-opt step, cost decreased to " << nextCost;

        return dist;
      }
    }
  }

  return (DifferenceType) -1;
}
