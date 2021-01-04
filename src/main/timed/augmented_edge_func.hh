#ifndef AUGMENTED_EDGE_FUNC_H
#define AUGMENTED_EDGE_FUNC_H

#include <map>

#include "timed_edge_func.hh"

class AugmentedEdgeFunc : public TimedEdgeFunc<num>
{
private:
  const Graph& graph;
  EdgeMap<ValueVector> timedCosts;
  num scaleFactor;

  void generateCosts(const EdgeFunc<num>& costs,
                     idx timeHorizon,
                     num scaleFactor,
                     const EdgeFunc<ValueVector>& times);

  void generateCosts(const Edge& edge,
                     num cost,
                     idx timeHorizon,
                     num scaleFactor,
                     const ValueVector& times);

public:
  AugmentedEdgeFunc(const Graph& graph,
                    const EdgeFunc<num>& costs,
                    idx timeHorizon,
                    num scaleFactor,
                    const EdgeFunc<ValueVector>& times)
    : graph(graph),
      timedCosts(graph, ValueVector{}),
      scaleFactor(scaleFactor)
  {
    assert(scaleFactor >= 1);

    generateCosts(costs, timeHorizon, scaleFactor, times);
  }

  num operator()(const Edge& edge, idx currentTime) const override
  {
    if(debuggingEnabled())
    {
      return timedCosts(edge).at(currentTime);
    }

    return timedCosts(edge)[currentTime];
  }

  template <class Generator>
  static AugmentedEdgeFunc generate(const Graph& graph,
                                    const EdgeFunc<num>& costs,
                                    num scaleFactor,
                                    idx steps,
                                    idx timeHorizon,
                                    Generator generator)
  {
    EdgeMap<ValueVector> times(graph, ValueVector());

    for(const Edge& edge : graph.getEdges())
    {
      for(idx i = 0; i < steps; ++i)
      {
        times(edge).push_back(generator());
      }

      times(edge).push_back(timeHorizon);

      std::sort(times(edge).begin(), times(edge).end());
    }


    return AugmentedEdgeFunc(graph,
                             costs,
                             timeHorizon,
                             scaleFactor,
                             times.getValues());
  }
};

#endif /* AUGMENTED_EDGE_FUNC_H */
