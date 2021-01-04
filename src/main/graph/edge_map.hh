#ifndef EDGE_MAP_HH
#define EDGE_MAP_HH

#include <cassert>
#include <iostream>

#include "graph/graph.hh"

/**
 * A class modelling a function defined on the edge set of a graph.
 */
template <class T>
class EdgeFunc
{
public:
  virtual T operator()(const Edge& edge) const = 0;

  virtual ~EdgeFunc() {}
};


/**
 * A comparator for edges based on a function mapping to comparable objects.
 */
template <class T>
class EdgeCompare
{
private:
  const EdgeFunc<T>& func;

public:
  EdgeCompare(const EdgeFunc<T>& func)
    : func(func) {}

  bool operator()(const Edge& first, const Edge& second) const
  {
    return func(first) < func(second);
  }
};

template <class T>
class SimpleEdgeFunc : public EdgeFunc<T>
{
private:
  std::function<T(const Edge&)> func;
public:
  SimpleEdgeFunc(const std::function<T(const Edge&)>& func)
    : func(func)
  {}

  T operator()(const Edge& edge) const override
  {
    return func(edge);
  }
};

template <class T>
class EdgeMap;

/**
 * A class which returns copies of the values of an underlying map.
 */
template <class T>
class EdgeValueMap : public EdgeFunc<T>
{
private:
  const EdgeMap<T> *map;

public:
  EdgeValueMap(const EdgeMap<T> & map) : map(&map) {}

  T operator()(const Edge& edge) const override
  {
    return T((*map)(edge));
  }

  ~EdgeValueMap() {}
};


/**
 * A map which explicitely stores its values.
 * The contents of the map can be modified.
 */
template <class T>
class EdgeMap : public EdgeFunc<const T&>
{
private:
  std::vector<T> values;
  EdgeValueMap<T> valueMap;

public:
  EdgeMap(const Graph& graph, T value)
    : values(graph.getEdges().size(), value),
      valueMap(*this)
  {
  }

  EdgeMap(const EdgeMap<T>& other)
    : values(other.values),
      valueMap(*this)
  {}

  EdgeMap(EdgeMap<T>&& other)
    : values(std::move(other.values)),
      valueMap(*this)
  {}

  EdgeMap(const Graph& graph, const EdgeFunc<T>& other)
    : valueMap(*this)
  {
    for(const Edge& edge : graph.getEdges())
    {
      values.push_back(other(edge));
    }
  }

  EdgeMap& operator=(const EdgeMap<T>& other)
  {
    values = other.values;
    valueMap = EdgeValueMap<T>(*this);

    return *this;
  }

  EdgeMap()
    : valueMap(*this)
  {}

  T& operator()(const Edge& edge)
  {
    assert(edge.getIndex() >= 0 and
           edge.getIndex() < values.size());
    return values[edge.getIndex()];
  }

  const T& operator()(const Edge& edge) const override
  {
    assert(edge.getIndex() >= 0 and
           edge.getIndex() < values.size());
    return values[edge.getIndex()];
  }

  void setValue(const Edge& edge, const T& value)
  {
    values[edge.getIndex()] = value;
  }

  void reset(const T& value)
  {
    std::fill(values.begin(), values.end(), value);
  }

  const EdgeValueMap<T>& getValues() const
  {
    return valueMap;
  }

  operator const EdgeValueMap<T>&() const
  {
    return getValues();
  }

  void extend(const Edge& edge, T value)
  {
    while(values.size() <= edge.getIndex())
    {
      values.push_back(value);
    }
  }
};

#endif /* EDGE_MAP_HH */
