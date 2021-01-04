#include "timed_lkh_solver.hh"

#include <algorithm>

#include "log.hh"


TimedLKHSolver::TimedLKHSolver(const Graph& graph,
                               const std::vector<Vertex>& vertices,
                               TimedDistanceFunc& distances,
                               ScoreFunction scoreFunction)
  : graph(graph),
    vertices(vertices),
    backTracingSteps(5),
    maxCandidates(5),
    distances(distances),
    scoreFunction(scoreFunction)
{
}

template<class It>
std::vector<Vertex> TimedLKHSolver::getCandidates(const num currentTime,
                                                  const Vertex currentVertex,
                                                  It it,
                                                  It end) const
{
  assert(it < end);

  Vertex nextVertex = *it;

  num nextScore = scoreFunction(currentVertex, nextVertex, nextVertex, currentTime);

  ++it;

  std::vector<Vertex> candidates;

  for(;it != end; ++it)
  {

    Vertex candidate = *it;

    num candidateScore = scoreFunction(currentVertex, nextVertex, candidate, currentTime);

    if(candidateScore < nextScore)
    {
      candidates.push_back(candidate);
    }
  }

  std::sort(candidates.begin(), candidates.end(),
            [&](const Vertex& first, const Vertex& second) -> bool
            {
              return scoreFunction(currentVertex, nextVertex, first, currentTime)
                < scoreFunction(currentVertex, nextVertex, second, currentTime);
            });

  if(candidates.size() > maxCandidates)
  {
    candidates.resize(maxCandidates);
  }

  assert(candidates.size() <= maxCandidates);

  return candidates;
}

void TimedLKHSolver::improve(std::vector<Vertex> head,
                             std::deque<Vertex> tail,
                             Tour& bestTour,
                             const num currentTime,
                             const idx steps) const
{
  assert(not(head.empty()));
  assert(head.size() + tail.size() == vertices.size());
  assert(currentTime >= 0);

  {
    TimedDistanceEvaluator evaluator(distances);

    std::vector<Vertex> nextVertices;

    nextVertices.reserve(vertices.size());

    nextVertices.insert(nextVertices.end(), head.begin(), head.end());
    nextVertices.insert(nextVertices.end(), tail.begin(), tail.end());

    Tour nextTour(graph, nextVertices);

    const num bestCosts = evaluator(bestTour);
    const num nextCosts = evaluator(nextTour);

    if(nextCosts < bestCosts)
    {
      Log(info) << "Found improvement from " << bestCosts
                << " to " << nextCosts
                << " after a series of " << steps
                << " 2-opt moves";

      bestTour = nextTour;
    }
  }

  if(tail.empty())
  {
    return;
  }

  Vertex currentVertex = *(head.rbegin());

  std::vector<Vertex> candidates = getCandidates(currentTime,
                                                 currentVertex,
                                                 tail.begin(),
                                                 tail.end());

  if(candidates.empty())
  {
    Vertex nextVertex = *(tail.begin());

    head.push_back(nextVertex);
    tail.pop_front();

    num nextTime = currentTime + distances(currentVertex, nextVertex, currentTime);

    improve(head, tail, bestTour, nextTime, steps);
  }
  else
  {
    if(steps >= backTracingSteps)
    {
      Vertex nextVertex = *(candidates.begin());

      head.push_back(nextVertex);

      std::deque<Vertex> nextTail;

      reverseUntil(tail.begin(), tail.end(), nextVertex, nextTail);

      assert(nextTail.size() + 1 == tail.size());

      num nextTime = currentTime + distances(currentVertex, nextVertex, currentTime);

      improve(head, nextTail, bestTour, nextTime, steps + 1);
    }
    else
    {
      for(const Vertex& nextVertex : candidates)
      {
        std::vector<Vertex> nextHead;

        nextHead.reserve(head.size() + 1);

        for(const Vertex& headVertex : head)
        {
          nextHead.push_back(headVertex);
        }

        nextHead.push_back(nextVertex);

        std::deque<Vertex> nextTail;

        reverseUntil(tail.begin(), tail.end(), nextVertex, nextTail);

        assert(nextTail.size() + 1 == tail.size());

        num nextTime = currentTime + distances(currentVertex, nextVertex, currentTime);

        improve(nextHead, nextTail, bestTour, nextTime, steps + 1);
      }
    }
  }
}

template<class It>
void TimedLKHSolver::reverseUntil(It begin,
                                  It end,
                                  const Vertex& vertex,
                                  std::deque<Vertex>& result) const
{
  assert(begin < end);

  It middle = std::find(begin, end, vertex);

  assert(middle < end);

  for(It current = begin; current != middle; ++current)
  {
    result.push_back(*current);
  }

  std::reverse(result.begin(), result.end());

  ++middle;

  for(It current = middle; current != end; ++current)
  {
    result.push_back(*current);
  }
}

Tour TimedLKHSolver::improveTour(const Tour& initialTour)
{
  Tour bestTour(initialTour);

  const std::vector<Vertex> tourVertices = bestTour.getVertices();

  for(idx i = 1; i < tourVertices.size(); ++i)
  {
    auto it = tourVertices.begin();

    std::vector<Vertex> head = {};

    for(idx j = 0; j < i; ++j)
    {
      head.push_back(*it);
      ++it;
    }

    std::deque<Vertex> tail(it, tourVertices.end());

    improve(head, tail, bestTour);
  }

  return bestTour;
}

TimedLKHSolver::ScoreFunction TimedLKHSolver::relativeDistances(DistanceFunc& staticDistances,
                                                                TimedDistanceFunc& distances)
{
  return [&](Vertex currentVertex, Vertex, Vertex candidate, num currentTime) -> double {
    return distances(currentVertex, candidate, currentTime)
      / ((double) staticDistances(currentVertex, candidate));
  };
}


TimedLKHSolver::ScoreFunction TimedLKHSolver::simpleDistances(TimedDistanceFunc& distances)
{
  return [&](Vertex currentVertex, Vertex, Vertex candidate, num currentTime) -> double {
    return distances(currentVertex, candidate, currentTime);
  };
}

TimedLKHSolver::ScoreFunction TimedLKHSolver::comparedDistances(TimedDistanceFunc& distances)
{
  return [&](Vertex currentVertex, Vertex nextVertex, Vertex candidate, num currentTime) -> double {
    return distances(currentVertex, candidate, currentTime)
      / ((double) distances(currentVertex, nextVertex, currentTime));
  };
}
