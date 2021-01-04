#ifndef SHIFTING_SOLVER_H
#define SHIFTING_SOLVER_H

#include "graph/graph.hh"

#include "timed/timed_vertex_func.hh"

#include "tour/tour.hh"

class ShiftingSolver
{
private:

  const Graph& graph;
  TimedDistanceFunc& distances;
  TimedDistanceEvaluator evaluator;

  typedef std::vector<Vertex>::const_iterator Iterator;

  std::vector<Vertex> flip(const std::vector<Vertex>& vertices,
                           Iterator begin,
                           Iterator end) const;

  std::vector<Vertex> swap(const std::vector<Vertex>& vertices,
                           Iterator begin,
                           Iterator end) const;

  bool improve(std::vector<Vertex>& bestVertices,
               num& bestCost,
               idx& distance) const;

public:
  ShiftingSolver(const Graph& graph,
                 TimedDistanceFunc& distances);

  Tour findTour(const Tour& initialTour);
};


#endif /* SHIFTING_SOLVER_H */
