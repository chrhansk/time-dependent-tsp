#ifndef TIME_EXPANDED_GRAPH_HH
#define TIME_EXPANDED_GRAPH_HH

#include <map>

#include "graph/graph.hh"

#include "graph/edge_map.hh"
#include "graph/vertex_map.hh"

#include "timed_vertex.hh"
#include "timed_edge.hh"
#include "timed_path.hh"

class TimeExpandedGraph
{
public:

  class EdgeIterator
  {
  private:
    const TimeExpandedGraph& graph;
    std::vector<Edge>::const_iterator it;

  public:
    EdgeIterator(const TimeExpandedGraph& graph,
                 std::vector<Edge>::const_iterator it)
      : graph(graph), it(it)
    {}

    TimedEdge operator*() const
    {
      const Edge& edge = *it;
      return TimedEdge(TimedVertex(edge.getSource().getIndex(),
                                   graph.times(edge.getSource())),
                       TimedVertex(edge.getTarget().getIndex(),
                                   graph.times(edge.getTarget())),
                       edge.getIndex());
    }

    const EdgeIterator& operator++()
    {
      ++it;
      return *this;
    }

    bool operator!=(const EdgeIterator& other)
    {
      return it != other.it;
    }

  };

  class Edges
  {
  private:
    const TimeExpandedGraph& graph;
    const std::vector<Edge>& edges;
  public:
    Edges(const TimeExpandedGraph& graph,
          const std::vector<Edge>& edges)
      : graph(graph), edges(edges)
    {}

    EdgeIterator begin() const
    {
      return EdgeIterator(graph, edges.begin());
    }

    EdgeIterator end() const
    {
      return EdgeIterator(graph, edges.end());
    }

    std::vector<TimedEdge> collect() const
    {
      std::vector<TimedEdge> timedEdges;
      timedEdges.reserve(edges.size());

      for(const TimedEdge& timedEdge : *this)
      {
        timedEdges.push_back(timedEdge);
      }

      return timedEdges;
    }

    bool empty() const
    {
      return edges.empty();
    }

    idx size() const
    {
      return edges.size();
    }
  };

  class VertexIterator
  {
  private:
    const TimeExpandedGraph& graph;
    Vertices::Iterator it;

  public:
    VertexIterator(const TimeExpandedGraph& graph,
                   Vertices::Iterator it)
      : graph(graph), it(it)
    {}

    TimedVertex operator*() const
    {
      const Vertex& vertex = *it;
      return TimedVertex(vertex.getIndex(), graph.times(vertex));
    }

    const VertexIterator& operator++()
    {
      ++it;
      return *this;
    }

    bool operator!=(const VertexIterator& other)
    {
      return it != other.it;
    }

  };

  class Vertices
  {
  private:
    const TimeExpandedGraph& graph;
  public:
    Vertices(const TimeExpandedGraph& graph)
      : graph(graph)
    {}

    VertexIterator begin() const
    {
      return VertexIterator(graph, ((const Graph&) graph).getVertices().begin());
    }

    VertexIterator end() const
    {
      return VertexIterator(graph, ((const Graph&) graph).getVertices().end());
    }

    std::vector<TimedVertex> collect() const
    {
      std::vector<TimedVertex> timedVertices;
      timedVertices.reserve(((const Graph&) graph).getVertices().size());

      for(const TimedVertex& timedVertex : *this)
      {
        timedVertices.push_back(timedVertex);
      }

      return timedVertices;
    }

    bool empty() const
    {
      return ((const Graph&) graph).getVertices().empty();
    }

    idx size() const
    {
      return ((const Graph&) graph).getVertices().size();
    }
  };

private:
  Graph graph;
  const Graph& underlying;
  VertexMap<std::map<idx, Vertex>> vertexTimes;

  VertexMap<Vertex> underlyingVertices;
  EdgeMap<Edge> underlyingEdges;
  EdgeMap<std::vector<TimedEdge>> timedEdges;

  mutable std::vector<TimedVertex> topologicalOrdering;

  VertexMap<idx> times;

public:
  TimeExpandedGraph(const Graph& underlyingGraph);

  TimedVertex addVertex(Vertex vertex, idx time);

  TimedEdge addEdge(TimedVertex source,
                    TimedVertex target,
                    Edge underlyingEdge);

  TimedEdge getEdge(Edge edge, idx time) const;

  TimedVertex getVertex(Vertex vertex, idx time) const;
  TimedVertex getVertex(Vertex vertex, idx time, bool add = false);

  TimedVertex asTimed(Vertex vertex) const;

  bool hasEdge(const Edge& edge, idx time) const;

  bool hasVertex(Vertex vertex, idx time) const;

  Vertex underlyingVertex(TimedVertex timedVertex) const;
  Vertex underlyingVertex(Vertex vertex) const;

  Edge underlyingEdge(TimedEdge timedEdge) const;
  Edge underlyingEdge(Edge edge) const;

  const std::vector<TimedVertex>& getTopologicalOrdering() const;

  const std::vector<TimedEdge> getTimedEdges(const Edge& underlyingEdge) const
  {
    return timedEdges(underlyingEdge);
  }

  operator const Graph&() const
  {
    return graph;
  }

  const Graph& underlyingGraph() const
  {
    return underlying;
  }

  const std::map<idx, Vertex>& getVertexTimes(Vertex underlyingVertex) const;

  const std::vector<TimedVertex> getExpandedVertices(Vertex underlyingVertex) const;

  TimedPath expandPath(const Path& path) const;

  Edges getOutgoing(TimedVertex timedVertex) const
  {
    return Edges(*this, graph.getOutgoing(Vertex(timedVertex.getIndex())));
  }

  Edges getIncoming(TimedVertex timedVertex) const
  {
    return Edges(*this, graph.getIncoming(Vertex(timedVertex.getIndex())));
  }

  Edges getEdges() const
  {
    return Edges(*this, graph.getEdges());
  }

  Vertices getVertices() const
  {
    return Vertices(*this);
  }

  SimpleEdgeFunc<num> travelTimes() const;

  template<class T>
  EdgeMap<T> combinedValues(const EdgeFunc<T>& values) const;
};

template<class T>
EdgeMap<T> TimeExpandedGraph::combinedValues(const EdgeFunc<T>& values) const
{
  const Graph& originalGraph = underlyingGraph();

  EdgeMap<T> combinedValues(originalGraph, (T) 0);

  for(const Edge& edge : originalGraph.getEdges())
  {
    for(const TimedEdge& timedEdge : getTimedEdges(edge))
    {
      combinedValues(edge) += values(timedEdge);
    }
  }

  return combinedValues;
}


#endif /* TIME_EXPANDED_GRAPH_HH */
