#include "cached_distances.hh"

num CachedDistances::operator()(const Vertex& source,
                                const Vertex& target,
                                idx departureTime)
{
  return cache.get(Entry{source, target, departureTime});
}

num CachedDistances::shortestPath(const Entry& entry)
{
  return router.travelTime(entry.source, entry.target, costs, entry.departureTime);
}
