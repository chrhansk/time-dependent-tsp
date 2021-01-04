#ifndef VERTEX_MAP_HH
#define VERTEX_MAP_HH

#include <functional>

#include "graph/graph.hh"

template <class T>
class VertexFunc
{
public:
  virtual T operator()(const Vertex& vertex) const = 0;

  virtual ~VertexFunc() {}
};

template <class T, bool decreasing = false>
class VertexCompare
{
private:
  const VertexFunc<T>& func;

public:
  VertexCompare(const VertexFunc<T>& func)
    : func(func) {}

  bool operator()(const Vertex& first, const Vertex& second) const
  {
    if(decreasing)
    {
      return func(first) > func(second);
    }
    else
    {
      return func(first) < func(second);
    }

  }
};

template <class T>
class SimpleVertexFunc : public VertexFunc<T>
{
private:
  std::function<T(const Vertex&)> func;
public:
  SimpleVertexFunc(const std::function<T(const Vertex&)>& func)
    : func(func)
  {}

  T operator()(const Vertex& vertex) const override
  {
    return func(vertex);
  }
};

template <class T>
class VertexMap;

template <class T>
class VertexValueMap : public VertexFunc<T>
{
private:
  const VertexMap<T> *map;

public:
  VertexValueMap(const VertexMap<T> & map) : map(&map) {}

  T operator()(const Vertex& vertex) const override
  {
    return T((*map)(vertex));
  }

  ~VertexValueMap() {}
};

template <class T>
class VertexMap : public VertexFunc<const T&>
{
private:
  std::vector<T> values;
  VertexValueMap<T> valueMap;

public:
  VertexMap(const Graph& graph, T value)
    : values(graph.getVertices().size(), value),
      valueMap(*this)
  {
  }

  VertexMap()
    : valueMap(*this)
  {}

  T& operator()(const Vertex& vertex)
  {
    return values[vertex.getIndex()];
  }

  const T& operator()(const Vertex& vertex) const override
  {
    return values[vertex.getIndex()];
  }

  void setValue(const Vertex& vertex, const T& value)
  {
    values[vertex.getIndex()] = value;
  }

  void addVertex(const Vertex& vertex, const T&value)
  {
    assert(values.size() == vertex.getIndex());
    values.push_back(value);
  }

  void reset(const T& value)
  {
    std::fill(values.begin(), values.end(), value);
  }

  const VertexValueMap<T>& getValues() const
  {
    return valueMap;
  }

  operator const VertexValueMap<T>&() const
  {
    return getValues();
  }
};

template <class T>
class BiVertexFunc
{
public:
  virtual T operator()(const Vertex& source, const Vertex& target) const = 0;
};

template <class T>
class SimpleBiVertexFunc : public BiVertexFunc<T>
{
private:
  const std::function<T(const Vertex&, const Vertex&)> func;
public:
  SimpleBiVertexFunc(const std::function<T(const Vertex&, const Vertex&)>& func)
    : func(func)
  {}

  virtual T operator()(const Vertex& source, const Vertex& target) const override
  {
    return func(source, target);
  }
};

typedef BiVertexFunc<num> DistanceFunc;
typedef SimpleBiVertexFunc<num> SimpleDistances;

class ExplicitDistances : public DistanceFunc
{
private:
  std::vector<Vertex> vertices;
  std::vector<std::vector<num>> values;

public:
  ExplicitDistances(const std::vector<Vertex>& vertices,
                  const std::vector<std::vector<num>>& values)
    : vertices(vertices),
      values(values)
  {}

  num operator()(const Vertex& source, const Vertex& target) const override
  {
    idx i = -1, j = -1, s = 0;

    for(auto it = vertices.begin(); it != vertices.end(); ++it, ++s)
    {
      if(*it == source)
      {
        i = s;
      }
      if(*it == target)
      {
        j = s;
      }
    }

    assert(i >= 0 and j >= 0);

    return values[i][j];
  }
};

#endif /* VERTEX_MAP_HH */
