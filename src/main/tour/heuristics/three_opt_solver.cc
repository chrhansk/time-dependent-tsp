#include "three_opt_solver.hh"

#include <iterator>

#include "log.hh"

ThreeOptSolver::ThreeOptSolver(const Graph& graph,
                               const std::vector<Vertex>& vertices)
  : graph(graph),
    vertices(vertices)
{
}

Tour ThreeOptSolver::findTour(TourEvaluator<>& evaluator, const Tour& initialTour)
{
  Tour currentTour(graph, initialTour.getVertices());

  improve(evaluator, currentTour);

  return currentTour;
}

void ThreeOptSolver::improve(TourEvaluator<>& evaluator,
                             Tour& tour)
{
  std::vector<Vertex>::iterator first = tour.getVertices().begin();

  const idx size = vertices.size();

  auto begin = tour.getVertices().begin();
  auto end = tour.getVertices().end();

  num currentCost = evaluator(tour);

  Log(info) << "Cost of initial tour: " << currentCost;

  for(; first != tour.getVertices().end(); ++first)
  {
    auto second = first;
    ++second;

    bool improved = false;

    for(; second != tour.getVertices().end(); ++second)
    {
      auto third = second;
      ++third;

      for(; third != tour.getVertices().end(); ++third)
      {
        // first slice: [begin, first)
        // second slice: [first, second)
        // third slice: [second, third)
        // last slice: [third, end)

        std::vector<Vertex> firstVertices;
        firstVertices.reserve(size);

        firstVertices.insert(std::end(firstVertices), begin, first);
        firstVertices.insert(std::end(firstVertices), second, third);
        firstVertices.insert(std::end(firstVertices),
                             std::reverse_iterator<decltype(second)>(second),
                             std::reverse_iterator<decltype(first)>(first));
        firstVertices.insert(std::end(firstVertices),
                             third, end);

        assert(firstVertices.size() == size);

        Tour firstTour(graph, firstVertices);

        const num firstCost = evaluator(firstTour);

        if(firstCost < currentCost)
        {
          Log(info) << "Found improvement, new tour cost: " << firstCost;
          std::copy(std::begin(firstVertices),
                    std::end(firstVertices),
                    std::begin(tour.getVertices()));
          currentCost = firstCost;
          break;
        }

        std::vector<Vertex> secondVertices;
        secondVertices.reserve(size);

        secondVertices.insert(std::end(secondVertices), begin, first);
        secondVertices.insert(std::end(secondVertices),
                              std::reverse_iterator<decltype(third)>(third),
                              std::reverse_iterator<decltype(second)>(second));
        secondVertices.insert(std::end(secondVertices),
                              std::reverse_iterator<decltype(second)>(second),
                              std::reverse_iterator<decltype(first)>(first));

        secondVertices.insert(std::end(secondVertices), third, end);

        assert(secondVertices.size() == size);

        Tour secondTour(graph, secondVertices);

        const num secondCost = evaluator(secondTour);

        if(secondCost < currentCost)
        {
          Log(info) << "Found improvement, new tour cost: " << secondCost;
          std::copy(std::begin(secondVertices),
                    std::end(secondVertices),
                    std::begin(tour.getVertices()));
          currentCost = secondCost;
          break;
        }

        std::vector<Vertex> thirdVertices;
        thirdVertices.reserve(size);

        thirdVertices.insert(std::end(thirdVertices), begin, first);
        thirdVertices.insert(std::end(thirdVertices), second, third);
        thirdVertices.insert(std::end(thirdVertices), first, second);

        thirdVertices.insert(std::end(thirdVertices), third, end);

        assert(thirdVertices.size() == size);

        Tour thirdTour(graph, thirdVertices);

        const num thirdCost = evaluator(thirdTour);

        if(thirdCost < currentCost)
        {
          Log(info) << "Found improvement, new tour cost: " << thirdCost;
          std::copy(std::begin(thirdVertices),
                    std::end(thirdVertices),
                    std::begin(tour.getVertices()));
          currentCost = thirdCost;
          break;
        }
      }

      if(improved)
      {
        break;
      }
    }

    auto next = first;
    ++next;
  }
}
