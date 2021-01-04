#ifndef TIMED_EDGE_FUNC_HH
#define TIMED_EDGE_FUNC_HH

#include "log.hh"

#include "graph/graph.hh"

#include "graph/edge_map.hh"

#include "timed_vertex_func.hh"

template <class T>
class TimedEdgeFunc
{
public:
  virtual T operator()(const Edge& edge, idx time) const = 0;
  virtual ~TimedEdgeFunc() {}
};

template <class T>
class IndependentEdgeFunc : public TimedEdgeFunc<T>
{
private:
  const EdgeFunc<T>& costs;
public:
  IndependentEdgeFunc(const EdgeFunc<T>& costs)
    : costs(costs)
  {}

  virtual T operator()(const Edge& edge, idx time) const override
  {
    return costs(edge);
  }
};

#endif /* TIMED_EDGE_FUNC_HH */
