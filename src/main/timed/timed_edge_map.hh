#ifndef TIMED_EDGE_MAP_HH
#define TIMED_EDGE_MAP_HH

#include <vector>

#include "graph/edge_map.hh"
#include "timed_edge_func.hh"

template <class T>
class TimedEdgeMap : public TimedEdgeFunc<T>
{
private:
  EdgeMap<std::vector<T>> values;
public:
  TimedEdgeMap(const Graph& graph)
    : values(graph, {})
  {}

  TimedEdgeMap()
  {}

  virtual T operator()(const Edge& edge, idx time) const override
  {
    return values(edge).at(time);
  }

  const std::vector<T>& edgeValues(const Edge& edge) const
  {
    return values(edge);
  }

  std::vector<T>& edgeValues(const Edge& edge)
  {
    return values(edge);
  }

};

#endif /* TIMED_EDGE_MAP_HH */
