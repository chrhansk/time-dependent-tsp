#ifndef COVERING_RELAXATION_HH
#define COVERING_RELAXATION_HH

#include "graph/graph.hh"

#include "timed/time_expanded_graph.hh"
#include "timed/timed_path.hh"

#include "tour/tour.hh"

struct RelaxationResult
{
  double dualBound;
  Tour bestTour;
};

class TimeExpandedGraph;

/**
 * A Lagrange relaxation based on penalizing
 * violations of covering constraints.
 **/
class CoveringRelaxation
{
private:
  const TimeExpandedGraph& graph;
  const Graph& originalGraph;

  std::vector<TimedVertex> timedVertices;

  Vertex source;

  VertexMap<double> penalties;

  TimedPath findPath() const;

  bool convertPath(const TimedPath& path,
                   TimedPath& convertedPath) const;

  Tour createTour(const TimedPath& path) const;

  double evaluate(const TimedPath& path) const;

public:
  CoveringRelaxation(const TimeExpandedGraph& graph,
                     Vertex source);

  RelaxationResult solve(idx maxIterations = 100);
};


#endif /* COVERING_RELAXATION_HH */
