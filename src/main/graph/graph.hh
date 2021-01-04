#ifndef GRAPH_HH
#define GRAPH_HH

#include <cassert>
#include <vector>
#include <queue>

#include "edge.hh"
#include "vertex.hh"
#include "vertex_set.hh"

class SubGraph;

/**
 * A class designed to iterate over the adjacent edges of a Vertex.
 *
 * @code
 *   for(const Edge& edge : graph.getAdjacentEdges(vertex))
 *   {...}
 * @endcode
 *
 **/
class AdjacentEdges
{
private:
  const std::vector<Edge>& outgoing;
  const std::vector<Edge>& incoming;
public:
  AdjacentEdges(const std::vector<Edge>& outgoing,
                const std::vector<Edge>& incoming)
    : outgoing(outgoing),
      incoming(incoming)
  {}

  class Iterator
  {
  private:
    const std::vector<Edge>& outgoing;
    const std::vector<Edge>& incoming;
    std::vector<Edge>::const_iterator iter;
  public:
    Iterator(const std::vector<Edge>& outgoing,
             const std::vector<Edge>& incoming,
             const std::vector<Edge>::const_iterator& iter)
      : outgoing(outgoing),
        incoming(incoming),
        iter(iter)
    {}
    Iterator(const std::vector<Edge>& outgoing,
             const std::vector<Edge>& incoming)
      : outgoing(outgoing),
        incoming(incoming),
        iter(outgoing.end())
    {}

    const Edge& operator*()
    {
      return *iter;
    }

    const Iterator& operator++()
    {
      if(++iter == incoming.end())
      {
        iter = outgoing.begin();
      }
      return *this;
    }

    const bool operator!=(const Iterator& other)
    {
      return iter != other.iter;
    }
  };

  Iterator begin()
  {
    return Iterator(outgoing, incoming,
                    incoming.empty() ? outgoing.begin() : incoming.begin());
  }
  Iterator end()
  {
    return Iterator(outgoing, incoming);
  }
};

/**
 * A class designed to iterate over the vertices of a graph.
 *
 * @code
 *   for(const Vertex& vertex : graph.getVertices())
 *   {...}
 * @endcode
 **/
class Vertices
{
private:
  idx _size;

public:
  Vertices(idx size)
    : _size(size)
  {}

  class Iterator
  {
  private:
    idx current;
  public:
    Iterator(idx current)
      : current(current)
    {}

    Vertex operator*() const
    {
      return Vertex(current);
    }

    const Iterator& operator++()
    {
      ++current;
      return *this;
    }

    const bool operator!=(const Iterator& other)
    {
      return current != other.current;
    }
  };

  Iterator begin()
  {
    return Iterator(0);
  }

  Iterator end()
  {
    return Iterator(size());
  }

  idx size()
  {
    return _size;
  }

  bool empty()
  {
    return size() == 0;
  }

  Vertex operator[](idx index)
  {
    return Vertex(index);
  }

  std::vector<Vertex> collect()
  {
    std::vector<Vertex> vertices;

    for(const Vertex& vertex : *this)
    {
      vertices.push_back(vertex);
    }

    return vertices;
  }

};

/**
 * A class modelling a graph. Vertices are stored implicitely.
 * Outgoing / incoming Edge%s are stored in vectors.
 **/
class Graph
{
private:
  idx size;
  std::vector<Edge> edges;

  std::vector<std::vector<Edge>> outgoing, incoming;

  bool check() const;

  void addEdge(const Edge& edge);

public:
  /**
   * Constructs a new Graph with the given vertices / Edge%s.
   *
   * @param size  The number of vertices in the graph.
   * @param edges The edges in the graph.
   **/
  Graph(idx size, const std::vector<Edge>& edges);
  Graph()
    : size(0),
      edges({})
  {}

  Vertex addVertex();

  Graph(idx size, std::initializer_list<std::pair<idx, idx>> edges);

  /**
   * Returns the Edge%s in this Graph.
   **/
  const std::vector<Edge>& getEdges() const;

  /**
   * Returns an iterator over the vertices of this Graph which
   * can be used in a range-based loop.
   **/
  Vertices getVertices() const;

  /**
   * Returns the outgoing Edge%s of the given Vertex.
   **/
  const std::vector<Edge>& getOutgoing(Vertex vertex) const;

  /**
   * Returns the incoming Edge%s of the given Vertex.
   **/
  const std::vector<Edge>& getIncoming(Vertex vertex) const;

  /**
   * Returns the all Edge%s incident to the given Vertex with
   * respect to a given Direction.
   **/
  const std::vector<Edge>& getEdges(Vertex vertex,
                                    Direction direction) const;

  /**
   * Returns an iterator over the Edge%s which are incident to
   * the given Vertex which
   * can be used in a range-based loop.
   **/
  AdjacentEdges getAdjacentEdges(const Vertex& vertex) const
  {
    return AdjacentEdges(getOutgoing(vertex),
                         getIncoming(vertex));
  }

  /**
   * Returns whether the Graph contains the given Edge.
   **/
  bool contains(const Edge& edge) const;

  /**
   * Adds an Edge between the given vertices to the Graph.
   * The added Edge is returned.
   **/
  Edge addEdge(Vertex source, Vertex target);


  /**
   * Applies the given Visitor function to all vertices
   * within the given hop length of the given Vertex.
   *
   * @tparam Visitor a parameter which can be called with a Vertex as argument
   *
   **/
  template <class Visitor>
  bool visitNeighbors(const Vertex& vertex,
                      idx neighborhoodSize,
                      Visitor visitor) const;

  VertexSet reachable(const Vertex& source) const;

  SubGraph inducedSubGraph(const std::vector<Vertex>& vertices) const;

  SubGraph reachableSubGraph(const std::vector<Vertex>& vertices) const;

  static Graph complete(idx numVertices);

};

template<class Visitor>
bool Graph::visitNeighbors(const Vertex& vertex,
                           idx neighborhoodSize,
                           Visitor visitor) const
{

  struct VisitedVertex
  {
    VisitedVertex(Vertex vertex, idx distance)
      : vertex(vertex),
        distance(distance)
    {}
    Vertex vertex;
    idx distance;
  };

  VertexSet visited(*this);
  std::queue<VisitedVertex> vertices;
  vertices.push(VisitedVertex(vertex, 0));
  visited.insert(vertex);

  while(!vertices.empty())
  {
    VisitedVertex current = vertices.front();
    vertices.pop();

    assert(visited.contains(current.vertex));

    if(!visitor(current.vertex))
    {
      return false;
    }

    if(current.distance == neighborhoodSize)
    {
      continue;
    }

    for(const Edge& edge : getAdjacentEdges(current.vertex))
    {
      Vertex other = edge.getOpposite(current.vertex);

      if(visited.contains(other))
      {
        continue;
      }

      visited.insert(other);
      vertices.push(VisitedVertex(other, current.distance + 1));

    }
  }

  return true;
}


template<class Engine>
std::vector<Vertex> selectRandomVertices(const Graph& graph,
                                         idx numVertices,
                                         Engine& engine)
{
  std::vector<Vertex> vertices = graph.getVertices().collect();

  std::shuffle(std::begin(vertices), std::end(vertices), engine);

  if(numVertices > vertices.size())
  {
    throw std::invalid_argument("Insufficient number of vertices");
  }

  vertices.resize(numVertices);

  return vertices;
}

#endif /* GRAPH_HH */
