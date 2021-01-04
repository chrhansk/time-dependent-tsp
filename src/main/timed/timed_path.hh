#ifndef TIMED_PATH_HH
#define TIMED_PATH_HH

#include <deque>
#include <memory>

#include "path/path.hh"

#include "timed_vertex.hh"
#include "timed_edge.hh"

class TimeExpandedGraph;

class TimedPath
{
public:

  struct PathVertexIterator
  {
    std::deque<TimedEdge>::const_iterator it;
    bool initial;

    TimedVertex operator*()
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
    const TimedPath& path;

    PathVertexIterator begin() const
    {
      return PathVertexIterator{path.getEdges().begin(), true};
    }

    PathVertexIterator end() const
    {
      return PathVertexIterator{path.getEdges().end(), false};
    }

    std::vector<TimedVertex> collect() const
    {
      std::vector<TimedVertex> vertices;
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
  std::shared_ptr<std::deque<TimedEdge>> edges;
public:

  /**
   * Constructs an empty Path.
   **/
  TimedPath() : edges(new std::deque<TimedEdge>()) {}

  /**
   * Constructs a path from the given TimedEdges%s.
   **/
  TimedPath(std::initializer_list<TimedEdge> edges);

  /**
   * Appends an TimedEdge to the given TimedPath.
   **/
  void append(const TimedEdge& edge);

  /**
   * Prepends an TimedEdge to the given TimedPath.
   **/
  void prepend(const TimedEdge& edge);

  void clear()
  {
    edges->clear();
  }

  /**
   * Returns the TimedEdge%s in this TimedPath.
   **/
  const std::deque<TimedEdge>& getEdges() const;

  /**
   * Adds an TimedEdge to this TimedPath from the given Direction. An
   * INCOMING TimedEdge is prepended, an OUTGOING TimedEdge is appended.
   **/
  void add(const TimedEdge& edge, Direction direction);

  idx travelTime() const;

  /**
   * Returns whether this TimedPath connects the given vertices.
   **/
  bool connects(TimedVertex source, TimedVertex target) const;

  /**
   * Returns whether this TimedPath connects the given vertices in the given
   * Direction.
   **/
  bool connects(TimedVertex source, TimedVertex target, Direction direction) const;

  /**
   * Returns whether this TimedPath contains the given TimedVertex.
   **/
  bool contains(TimedVertex vertex) const;

  /**
   * Returns whether this TimedPath contains the given TimedEdge.
   **/
  bool contains(TimedEdge edge) const;

  /**
   * Returns whether this TimedPath is simple, i.e. whether it does
   * not contains cycles.
   **/
  bool isSimple() const;

  /**
   * Returns the source TimedVertex of this TimedPath.
   **/
  TimedVertex getSource() const;

  /**
   * Returns the target TimedVertex of this TimedPath.
   **/
  TimedVertex getTarget() const;

  Path underlyingPath(const TimeExpandedGraph& graph) const;

  bool isEmpty() const
  {
    return getEdges().empty();
  }

  bool operator==(const TimedPath& other) const
  {
    return getEdges() == other.getEdges();
  }

  bool operator!=(const TimedPath& other) const
  {
    return getEdges() != other.getEdges();
  }

  idx girth(const TimeExpandedGraph& graph) const;

  template <class T>
  T cost(const EdgeFunc<T>& func) const;

  PathVertices getVertices() const
  {
    return PathVertices{*this};
  }
};

template <class T>
T TimedPath::cost(const EdgeFunc<T>& func) const
{
  T s = 0;

  for(const TimedEdge& edge : getEdges())
  {
    s += func(edge);
  }

  return s;
}

namespace std
{
  template<>
  struct hash<TimedPath>
  {
    typedef std::size_t result_type;

    result_type operator()(const TimedPath& timedPath) const
    {
      result_type seed = 0;

      std::hash<TimedEdge> timedEdgeHash{};

      for(const TimedEdge& timedEdge : timedPath.getEdges())
      {
        compute_hash_combination(seed, timedEdgeHash(timedEdge));
      }

      return seed;
    }
  };
}

#endif /* TIMED_PATH_HH */
