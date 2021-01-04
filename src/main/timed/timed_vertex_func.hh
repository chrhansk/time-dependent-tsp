#ifndef TIMED_VERTEX_FUNC_HH
#define TIMED_VERTEX_FUNC_HH

#include "graph/graph.hh"

#include "graph/vertex_map.hh"

template <class T>
class TimedVertexFunc
{
public:
  virtual T operator()(const Vertex& vertex, idx time) const = 0;
  virtual ~TimedVertexFunc() {}
};

template <class T>
class IndependentVertexFunc : public TimedVertexFunc<T>
{
private:
  const VertexFunc<T>& costs;
public:
  IndependentVertexFunc(const VertexFunc<T>& costs)
    : costs(costs)
  {}

  virtual T operator()(const Vertex& vertex, idx time) const
  {
    return costs(vertex);
  }
};

template <class T>
class TimedBiVertexFunc
{
public:
  virtual T operator()(const Vertex& source, const Vertex& target, idx departureTime) = 0;
  virtual ~TimedBiVertexFunc() {}
};

template <class T>
class ChainedTimedBiVertexFunc : public TimedBiVertexFunc<T>
{
private:
  TimedBiVertexFunc<T>& func;
  std::function<Vertex(const Vertex&)> before;
public:

  ChainedTimedBiVertexFunc(TimedBiVertexFunc<T>& func,
                           const std::function<Vertex(const Vertex&)>& before)
    : func(func),
      before(before)
  {}

  virtual T operator()(const Vertex& source, const Vertex& target, idx departureTime) override
  {
    return func(before(source), before(target), departureTime);
  }
};

typedef TimedBiVertexFunc<num> TimedDistanceFunc;

#endif /* TIMED_VERTEX_FUNC_HH */
