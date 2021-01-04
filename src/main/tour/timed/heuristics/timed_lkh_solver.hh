#ifndef TIMED_LKH_SOLVER_HH
#define TIMED_LKH_SOLVER_HH

#include <deque>

#include "graph/graph.hh"

#include "timed/timed_vertex_func.hh"

#include "tour/tour.hh"

class TimedLKHSolver
{
public:
  typedef std::function<double(Vertex, Vertex, Vertex, num)> ScoreFunction;
private:
  const Graph& graph;
  const std::vector<Vertex> vertices;
  idx backTracingSteps;
  idx maxCandidates;
  TimedDistanceFunc& distances;
  ScoreFunction scoreFunction;

  template<class It>
  std::vector<Vertex> getCandidates(const num currentTime,
                                    const Vertex currentVertex,
                                    It begin,
                                    It end) const;

  void improve(std::vector<Vertex> head,
               std::deque<Vertex> tail,
               Tour& bestTour,
               const num currentTime = 0,
               const idx steps = 0) const;

  template<class It>
  void reverseUntil(It begin,
                    It end,
                    const Vertex& vertex,
                    std::deque<Vertex>& result) const;

public:
  TimedLKHSolver(const Graph& graph,
                 const std::vector<Vertex>& vertices,
                 TimedDistanceFunc& distances,
                 ScoreFunction scoreFunction);

  Tour improveTour(const Tour& initialTour);

  static ScoreFunction relativeDistances(DistanceFunc& staticDistances,
                                         TimedDistanceFunc& distances);

  static ScoreFunction simpleDistances(TimedDistanceFunc& distances);

  static ScoreFunction comparedDistances(TimedDistanceFunc& distances);
};

#endif /* TIMED_LKH_SOLVER_HH */
