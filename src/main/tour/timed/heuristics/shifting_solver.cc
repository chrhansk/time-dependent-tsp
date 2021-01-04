#include "shifting_solver.hh"


ShiftingSolver::ShiftingSolver(const Graph& graph,
                               TimedDistanceFunc& distances)
  : graph(graph),
    distances(distances),
    evaluator(distances)
{}

std::vector<Vertex> ShiftingSolver::flip(const std::vector<Vertex>& vertices,
                                         ShiftingSolver::Iterator first,
                                         ShiftingSolver::Iterator second) const
{
  assert(first < second);

  std::vector<Vertex> nextVertices;
  nextVertices.reserve(vertices.size());

  nextVertices.insert(std::end(nextVertices),
                      std::begin(vertices),
                      first);

  nextVertices.insert(std::end(nextVertices),
                      std::reverse_iterator<decltype(second)>(second),
                      std::reverse_iterator<decltype(first)>(first));

  nextVertices.insert(std::end(nextVertices),
                      second,
                      std::end(vertices));

  assert(nextVertices.size() == vertices.size());

  return nextVertices;
}

std::vector<Vertex> ShiftingSolver::swap(const std::vector<Vertex>& vertices,
                                         ShiftingSolver::Iterator first,
                                         ShiftingSolver::Iterator second) const
{
  assert(first < second);

  std::vector<Vertex> nextVertices;
  nextVertices.reserve(vertices.size());

  nextVertices.insert(std::end(nextVertices),
                      std::begin(vertices),
                      first);

  nextVertices.push_back(*second);

  {
    auto it = first;
    ++it;

    nextVertices.insert(std::end(nextVertices),
                        it,
                        second);
  }

  nextVertices.push_back(*first);

  {
    auto it = second;
    ++it;

    nextVertices.insert(std::end(nextVertices),
                        it,
                        std::end(vertices));
  }

  assert(nextVertices.size() == vertices.size());

  return nextVertices;
}

bool ShiftingSolver::improve(std::vector<Vertex>& bestVertices,
                             num& bestCost,
                             idx& distance) const
{
  TimedDistanceEvaluator evaluator(distances);

  std::vector<Vertex> currentVertices;
  currentVertices.reserve(bestVertices.size());

  auto first = std::begin(bestVertices);
  std::advance(first, distance);

  for(;first != std::end(bestVertices); ++first, ++distance)
  {
    auto second = first;
    ++second;

    for(; second != std::end(bestVertices); ++second)
    {

      {
        auto nextVertices = swap(bestVertices, first, second);

        Tour nextTour(graph, nextVertices);
        num nextCost = evaluator(nextTour);

        if(nextCost < bestCost)
        {
          bestCost = nextCost;
          bestVertices = nextVertices;
          return true;
        }
      }

      {

        auto nextVertices = flip(bestVertices, first, second);

        Tour nextTour(graph, nextVertices);
        num nextCost = evaluator(nextTour);

        if(nextCost < bestCost)
        {
          bestCost = nextCost;
          bestVertices = nextVertices;
          return true;
        }
      }
    }
  }

  return false;
}

Tour ShiftingSolver::findTour(const Tour& initialTour)
{
  assert(initialTour.connects(graph.getVertices().collect()));

  num bestCost = evaluator(initialTour);
  std::vector<Vertex> bestVertices = initialTour.getVertices();

  idx distance = 1;

  while(true)
  {
    std::vector<Vertex> currentVertices = bestVertices;

    if(improve(currentVertices, bestCost, distance))
    {
      bestVertices = currentVertices;
      Log(info) << "Tour costs dropped to " << bestCost;
    }
    else
    {
      break;
    }

  }

  Tour bestTour(graph, bestVertices);

  assert(bestTour.connects(graph.getVertices().collect()));
  assert(bestTour.getSource() == initialTour.getSource());

  return bestTour;
}
