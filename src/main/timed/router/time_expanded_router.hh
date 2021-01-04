#ifndef TIME_EXPANDED_ROUTER_HH
#define TIME_EXPANDED_ROUTER_HH

#include <optional>

#include "graph/edge_set.hh"

#include "timed/time_expanded_graph.hh"
#include "timed/timed_path.hh"

class TimeExpandedRouter
{
protected:
  const TimeExpandedGraph& graph;
  Vertex originalSource;

  class ReverseLabel
  {
  private:
    double cost;
    std::optional<TimedEdge> timedEdge;
    TimedVertex timedVertex;
  public:
    ReverseLabel() : cost(inf) {}

    ReverseLabel(double cost,
                 const TimedVertex& timedVertex)
      : cost(cost),
        timedVertex(timedVertex)
    {}

    ReverseLabel(double cost,
                 const TimedEdge& timedEdge)
      : cost(cost),
        timedEdge(timedEdge),
        timedVertex(timedEdge.getSource())
    {}

    ReverseLabel& operator=(const ReverseLabel& other) = default;

    double getCost() const
    {
      return cost;
    }

    const TimedVertex& getTimedVertex() const
    {
      return timedVertex;
    }

    const std::optional<TimedEdge>& getTimedEdge() const
    {
      return timedEdge;
    }

    bool operator<(const ReverseLabel& other) const
    {
      return getCost() < other.getCost();
    }
  };

public:

  TimeExpandedRouter(const TimeExpandedGraph& graph,
                     Vertex originalSource)
    : graph(graph),
      originalSource(originalSource)
  {}

  struct Request
  {
    Request(const EdgeFunc<double>& costs,
            const EdgeSet& forbiddenEdges)
      : forbiddenEdges(forbiddenEdges),
        costs(costs)
    {}

    EdgeSet forbiddenEdges;

    const EdgeFunc<double>& costs;

    std::optional<double> cutoffCost;

    std::optional<double> lowerTimeBound;
    std::optional<double> upperTimeBound;

    std::optional<idx> maxNumPaths;
  };

  struct Result
  {
    std::vector<TimedPath> paths;
    std::optional<double> minCost;
  };

  virtual Result findShortestPaths(const Request& request) = 0;

  VertexMap<ReverseLabel> findLowerBounds(const Request& request) const;

};


#endif /* TIME_EXPANDED_ROUTER_HH */
