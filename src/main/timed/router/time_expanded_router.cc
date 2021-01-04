#include "time_expanded_router.hh"

VertexMap<TimeExpandedRouter::ReverseLabel>
TimeExpandedRouter::findLowerBounds(const Request& request) const
{
  VertexMap<ReverseLabel> labels(graph, ReverseLabel());

  for(const TimedVertex& timedVertex : graph.getExpandedVertices(originalSource))
  {
    if(timedVertex.getTime() == 0)
    {
      continue;
    }

    if(cmp::gt(timedVertex.getTime(), request.upperTimeBound.value_or(inf)))
    {
      continue;
    }

    if(cmp::lt(timedVertex.getTime(), request.lowerTimeBound.value_or(-inf)))
    {
      continue;
    }

    labels(timedVertex) = ReverseLabel(0., timedVertex);
  }

  const auto& ordering = graph.getTopologicalOrdering();

  for(auto it = ordering.rbegin(); it != ordering.rend(); ++it)
  {
    const TimedVertex& timedVertex = *it;

    auto currentLabel = labels(timedVertex);

    if(currentLabel.getCost() == inf)
    {
      continue;
    }

    for(const TimedEdge& incoming : graph.getIncoming(timedVertex))
    {
      if(request.forbiddenEdges.contains(graph.underlyingEdge(incoming)))
      {
        continue;
      }

      auto& nextLabel = labels(incoming.getSource());

      const double edgeCost = request.costs(incoming);

      const double nextCost = currentLabel.getCost() + edgeCost;

      if(nextLabel.getCost() > nextCost)
      {
        nextLabel = ReverseLabel(nextCost, incoming);
      }

      assert(nextLabel.getTimedVertex() == incoming.getSource());
    }

  }

  return labels;
}
