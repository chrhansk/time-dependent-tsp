#ifndef INSTANCE_HH
#define INSTANCE_HH

#include <optional>

#include "graph/edge_map.hh"
#include "graph/vertex_map.hh"
#include "graph/graph.hh"

#include "timed/cached_distances.hh"
#include "timed/cached_tree_distances.hh"
#include "timed/augmented_edge_func.hh"

struct InstanceInfo
{
  int numVertices;
  idx seed;

  num timeSteps;

  num scaleFactor;
  num numBreaks;

  std::optional<num> optimalObjective;

  std::optional<double> relaxedObjective;

  std::vector<double> relaxedAcyclicObjectives;

  InstanceInfo(idx seed,
               idx numVertices,
               std::optional<num> optimalObjective = {},
               std::optional<double> relaxedObjective = {},
               const std::vector<double>& relaxedAcyclicObjectives = {},
               idx timeSteps = 3600)
    : numVertices(numVertices),
      seed(seed),
      timeSteps(timeSteps),
      scaleFactor(3),
      numBreaks(timeSteps / 10),
      optimalObjective(optimalObjective),
      relaxedObjective(relaxedObjective),
      relaxedAcyclicObjectives(relaxedAcyclicObjectives)
  {}

  static std::vector<InstanceInfo> tinyInstances();

  static std::vector<InstanceInfo> smallInstances();

  static std::vector<InstanceInfo> mediumInstances();

  static std::vector<InstanceInfo> largeInstances();

  std::optional<double> relaxedAcyclicObjective(idx size) const
  {
    assert(size >= 2);
    size -= 2;

    if(size >= relaxedAcyclicObjectives.size())
    {
      return {};
    }

    return relaxedAcyclicObjectives.at(size);
  }
};

struct SimpleInstance
{
  InstanceInfo info;
  std::mt19937 engine;
  Graph graph;
  EdgeMap<num> costs;

  Dijkstra<> dijkstra;
  ShortestPathDistances<> staticCosts;

  SimpleInstance(const InstanceInfo& info);

};

struct Instance : public SimpleInstance
{
  std::uniform_int_distribution<int> distribution;
  AugmentedEdgeFunc timedCosts;
  CachedTreeDistances timedDistances;

  Instance(const InstanceInfo& info);
};

#endif /* INSTANCE_HH */
