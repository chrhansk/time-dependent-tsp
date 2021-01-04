#ifndef PATH_HH
#define PATH_HH

#include <deque>
#include <memory>

#include "graph/edge.hh"
#include "graph/edge_map.hh"

#include "timed/timed_edge_func.hh"
#include "timed/timed_vertex_func.hh"

/**
 * A Path given by a tuple of Edge%s. Each successive
 * pair of Edge%s of the Path shares a common Vertex.
 * A Path may contain cylces.
 **/
class Path
{
public:

  struct PathVertexIterator
  {
    std::deque<Edge>::const_iterator it;
    bool initial;

    Vertex operator*()
    {
      return initial ? it->getSource() : it->getTarget();
    }

    const PathVertexIterator& operator++()
    {
      if(initial)
      {
        initial = false;
      }
      else
      {
        ++it;
      }

      return *this;
    }

    const bool operator!=(const PathVertexIterator& other)
    {
      return (it != other.it) || (initial != other.initial);
    }
  };

  struct PathVertices
  {
    const Path& path;

    PathVertexIterator begin() const
    {
      return PathVertexIterator{path.getEdges().begin(), true};
    }

    PathVertexIterator end() const
    {
      return PathVertexIterator{path.getEdges().end(), false};
    }

    std::vector<Vertex> collect() const
    {
      std::vector<Vertex> vertices;
      for(auto it = begin(); it != end(); ++it)
      {
        vertices.push_back(*it);
      }

      return vertices;
    }

    idx size() const
    {
      return path.getEdges().size();
    }
  };

private:
  std::shared_ptr<std::deque<Edge>> edges;
public:

  /**
   * Constructs an empty Path.
   **/
  Path() : edges(new std::deque<Edge>()) {}

  /**
   * Constructs a path from the given Edges%s.
   **/
  Path(std::initializer_list<Edge> edges);

  /**
   * Appends an Edge to the given Path.
   **/
  void append(const Edge& edge);

  /**
   * Prepends an Edge to the given Path.
   **/
  void prepend(const Edge& edge);

  /**
   * Returns the cost of the path with respect to
   * the given cost function.
   *
   * @tparam A cost function given as a map from Edge%s to numbers.
   *
   **/
  template <class T>
  num cost(const EdgeFunc<T>& func) const;

  template <class T>
  num cost(const TimedEdgeFunc<T>& func, idx departureTime = 0) const;

  template <class T>
  num arrivalTime(const TimedEdgeFunc<T>& func, idx departureTime = 0) const;

  template <class T>
  num cost(TimedBiVertexFunc<T>& func, idx departureTime = 0) const;

  template <class T>
  num arrivalTime(TimedBiVertexFunc<T>& func, idx departureTime = 0) const;

  /**
   * Returns the Edge%s in this Path.
   **/
  const std::deque<Edge>& getEdges() const;

  /**
   * Adds an Edge to this Path from the given Direction. An
   * INCOMING Edge is prepended, an OUTGOING Edge is appended.
   **/
  void add(const Edge& edge, Direction direction);

  /**
   * Returns whether this Path connects the given vertices.
   **/
  bool connects(Vertex source, Vertex target) const;

  /**
   * Returns whether this Path connects the given vertices in the given
   * Direction.
   **/
  bool connects(Vertex source, Vertex target, Direction direction) const;

  /**
   * Returns whether this Path contains the given Vertex.
   **/
  bool contains(Vertex vertex) const;

  /**
   * Returns whether this Path contains the given Edge.
   **/
  bool contains(Edge edge) const;

  /**
   * Returns whether this Path is simple, i.e. whether it does
   * not contains cycles.
   **/
  bool isSimple() const;

  /**
   * Returns the source Vertex of this Path.
   **/
  Vertex getSource() const;

  /**
   * Returns the target Vertex of this Path.
   **/
  Vertex getTarget() const;

  bool isTour(const Graph& graph) const;

  /**
   * Returns the endpoint of this Path with respect to
   * the given Direction-
   **/
  Vertex getEndpoint(Direction direction) const;

  /**
   * Returns whether this Path satisfies the given filter.
   * @tparam Filter A filer given by a map from Edge%s to boolean values.
   **/
  template <class Filter>
  bool satisfies(Filter& filter) const;

  /**
   * Returns whether this Path decomposes into two subpaths such
   * that the subpath containing the source satisfies the given
   * forward filter and the one containing the target satisfies
   * the backward filter.
   *
   * @tparam ForwardFilter  A filer given by a map from Edge%s to boolean values.
   * @tparam BackwardFilter A filer given by a map from Edge%s to boolean values.
   **/
  template <class ForwardFilter, class BackwardFilter>
  bool satisfies(ForwardFilter& forwardFilter,
                 BackwardFilter& backwardFilter) const;

  /**
   * Returns whether this path contains any Edge%s.
   **/
  operator bool() const
  {
    return !(getEdges().empty());
  }

  PathVertices getVertices() const
  {
    return PathVertices{*this};
  }
};

template <class T>
num Path::cost(const EdgeFunc<T>& func) const
{
  num s = 0;

  for(const Edge& edge : getEdges())
  {
    s += func(edge);
  }

  return s;
}

template <class T>
num Path::cost(const TimedEdgeFunc<T>& func, idx departureTime) const
{
  num s = 0;
  num currentTime = departureTime;

  for(const Edge& edge : getEdges())
  {
    num currentCost = func(edge, currentTime);

    currentTime += currentCost;
    s += currentCost;
  }

  return s;
}

template <class T>
num Path::arrivalTime(const TimedEdgeFunc<T>& func, idx departureTime) const
{
  num currentTime = departureTime;

  for(const Edge& edge : getEdges())
  {
    num currentCost = func(edge, currentTime);

    currentTime += currentCost;
  }

  return currentTime;
}

template <class T>
num Path::cost(TimedBiVertexFunc<T>& func, idx departureTime) const
{
  num s = 0;
  num currentTime = departureTime;

  for(const Edge& edge : getEdges())
  {
    num currentCost = func(edge.getSource(), edge.getTarget(), currentTime);

    currentTime += currentCost;
    s += currentCost;
  }

  return s;
}

template <class T>
num Path::arrivalTime(TimedBiVertexFunc<T>& func, idx departureTime) const
{
  num currentTime = departureTime;

  for(const Edge& edge : getEdges())
  {
    num currentCost = func(edge.getSource(), edge.getTarget(), currentTime);

    currentTime += currentCost;
  }

  return currentTime;
}


template <class Filter>
bool Path::satisfies(Filter& filter) const
{
  for(const Edge& edge : getEdges())
  {
    if(!filter(edge))
    {
      return false;
    }
  }

  return true;
}

template <class ForwardFilter, class BackwardFilter>
bool Path::satisfies(ForwardFilter& forwardFilter,
                     BackwardFilter& backwardFilter) const
{
  bool forward = true;

  for(const Edge& edge : getEdges())
  {
    if(forward)
    {
      if(!forwardFilter(edge))
      {
        if(!backwardFilter(edge))
        {
          return false;
        }

        forward = false;
      }
    }
    else if(!backwardFilter(edge))
    {
      return false;
    }
  }

  return true;
}



#endif /* PATH_HH */
