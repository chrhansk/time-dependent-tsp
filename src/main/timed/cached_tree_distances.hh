#ifndef CACHED_TREE_DISTANCES_HH
#define CACHED_TREE_DISTANCES_HH

#include <list>
#include <unordered_map>
#include <vector>

#include "timed_vertex_func.hh"
#include "timed_router.hh"

#include "cache.hh"
#include "util.hh"

class CachedTreeDistances : public TimedDistanceFunc
{
private:

  struct Entry
  {
    Vertex source;
    Vertex target;
    idx departureTime;

    bool operator==(const Entry& other) const
    {
      return source == other.source and
        target == other.target and
        departureTime == other.departureTime;
    }
  };

  struct EntryHasher
  {
    std::size_t operator()(const Entry& entry) const
    {
      std::size_t seed = 0;
      compute_hash_combination(seed, entry.source);
      compute_hash_combination(seed, entry.target);
      compute_hash_combination(seed, entry.departureTime);

      return seed;
    }
  };

  const Graph& graph;
  std::vector<Vertex> vertices;
  TimedEdgeFunc<num>& costs;
  Cache<Entry, num, EntryHasher> cache;

  num shortestPath(const Entry& entry);

public:
  CachedTreeDistances(const Graph& graph,
                      const std::vector<Vertex>& vertices,
                      TimedEdgeFunc<num>& costs,
                      idx capacity = 100000)
    : graph(graph),
      vertices(vertices),
      costs(costs),
      cache(std::bind(&CachedTreeDistances::shortestPath, this, std::placeholders::_1), capacity)
  {}

  num operator()(const Vertex& vertex, const Vertex& target, idx departureTime) override;
};

#endif /* CACHED_TREE_DISTANCES_HH */
