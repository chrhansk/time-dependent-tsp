#include "timed_router.hh"

SearchResult<>
TimedDijkstra::shortestPath(Vertex source,
                            Vertex target,
                            const TimedEdgeFunc<num>& costs,
                            idx startingTime)
{
  return shortestPath<AllEdgeFilter, false>(source,
                                            target,
                                            costs,
                                            startingTime,
                                            AllEdgeFilter(),
                                            inf);
}
