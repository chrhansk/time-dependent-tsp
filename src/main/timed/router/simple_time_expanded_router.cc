#include "simple_time_expanded_router.hh"

#include <optional>

#include "timed_path_set.hh"

namespace
{
  class Label
  {
  private:
    double cost;
    std::optional<TimedEdge> timedEdge;
    TimedVertex timedVertex;
  public:
    Label() : cost(inf) {}

    Label(double cost,
          const TimedVertex& timedVertex)
      : cost(cost),
        timedVertex(timedVertex)
    {}

    Label(double cost,
          const TimedEdge& timedEdge)
      : cost(cost),
        timedEdge(timedEdge),
        timedVertex(timedEdge.getTarget())
    {}

    Label& operator=(const Label& other) = default;

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

    bool operator<(const Label& other) const
    {
      return getCost() < other.getCost();
    }
  };
}

TimeExpandedRouter::Result
SimpleTimeExpandedRouter::findShortestPaths(const Request& request)
{
  Result result;

  TimedVertex timedSource = graph.getVertex(originalSource, 0);

  VertexMap<Label> labels(graph, Label());

  Label sourceLabel(0, timedSource);

  labels(timedSource) = sourceLabel;

  std::optional<double> minCost;

  TimedPathSet bestPaths(request.maxNumPaths);

  auto createPath = [&](const Label& label) -> TimedPath
    {
      Label currentLabel = label;

      TimedPath timedPath;

      while(currentLabel.getTimedEdge())
      {
        const TimedEdge& timedEdge = *(currentLabel.getTimedEdge());

        timedPath.prepend(timedEdge);

        currentLabel = labels(timedEdge.getSource());
      }

      return timedPath;
    };

  for(const TimedVertex& timedVertex : graph.getTopologicalOrdering())
  {
    Label currentLabel = labels(timedVertex);

    // if there are forbidden edges, not all vertices
    // need to be reachable.
    if(currentLabel.getCost() == inf)
    {
      continue;
    }

    assert(currentLabel.getTimedVertex() == timedVertex);

    if(graph.underlyingVertex(timedVertex) == originalSource &&
       timedVertex.getTime() > 0)
    {
      assert(graph.getOutgoing(timedVertex).size() == 0);
      assert(timedVertex != timedSource);

      if(cmp::gt(timedVertex.getTime(), request.upperTimeBound.value_or(inf)))
      {
        continue;
      }

      if(cmp::lt(timedVertex.getTime(), request.lowerTimeBound.value_or(-inf)))
      {
        continue;
      }

      if(cmp::lt(currentLabel.getCost(), request.cutoffCost.value_or(inf)))
      {
        minCost = std::min(minCost.value_or(inf), currentLabel.getCost());

        if(currentLabel.getCost() < bestPaths.cutoffValue().value_or(inf))
        {
          bestPaths.insert(createPath(currentLabel),
                           currentLabel.getCost());
        }
      }

      continue;
    }

    for(const TimedEdge& outgoing : graph.getOutgoing(timedVertex))
    {
      if(request.forbiddenEdges.contains(graph.underlyingEdge(outgoing)))
      {
        continue;
      }

      Label& nextLabel = labels(outgoing.getTarget());

      const double edgeCost = request.costs(outgoing);

      const double nextCost = currentLabel.getCost() + edgeCost;

      if(nextLabel.getCost() > nextCost)
      {
        nextLabel = Label(nextCost, outgoing);
      }

      assert(nextLabel.getTimedVertex() == outgoing.getTarget());
    }
  }

  result.minCost = bestPaths.bestValue();

  for(const TimedPath& timedPath : bestPaths.getPaths())
  {
    result.paths.push_back(timedPath);
  }

  if(minCost && cmp::lt(*minCost, request.cutoffCost.value_or(inf)))
  {
    result.minCost = minCost;
  }

  return result;
}
