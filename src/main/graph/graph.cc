#include "graph.hh"

#include <cassert>
#include <stack>

#include "subgraph.hh"
#include "vertex_set.hh"

Graph::Graph(idx size, const std::vector<Edge>& edges)
  : size(size)
{
  for(idx i = 0; i < size; ++i)
  {
    outgoing.push_back(std::vector<Edge>());
    incoming.push_back(std::vector<Edge>());
  }

  for(const Edge& edge : edges)
  {
    addEdge(edge);
  }

  //assert(check());
}

Graph::Graph(idx size, std::initializer_list<std::pair<idx, idx>> edges)
  : size(size)
{
  for(idx i = 0; i < size; ++i)
  {
    outgoing.push_back(std::vector<Edge>());
    incoming.push_back(std::vector<Edge>());
  }

  idx i = 0;
  for(const auto& pair : edges)
  {
    addEdge(Edge(Vertex(pair.first), Vertex(pair.second), i++));
  }
}

Vertex Graph::addVertex()
{
  Vertex vertex(size);
  outgoing.push_back({});
  incoming.push_back({});
  ++size;
  return vertex;
}

void Graph::addEdge(const Edge& edge)
{
  assert(edge.getIndex() == edges.size());
  assert(edge.getSource().getIndex() < size);
  assert(edge.getTarget().getIndex() < size);

  outgoing[edge.getSource().getIndex()].push_back(edge);
  incoming[edge.getTarget().getIndex()].push_back(edge);

  edges.push_back(Edge(edge));
}

const std::vector<Edge>& Graph::getEdges() const
{
  return edges;
}

Vertices Graph::getVertices() const
{
  return Vertices(size);
}

const std::vector<Edge>& Graph::getOutgoing(Vertex vertex) const
{
  return outgoing[vertex.getIndex()];
}

const std::vector<Edge>& Graph::getIncoming(Vertex vertex) const
{
  return incoming[vertex.getIndex()];
}

const std::vector<Edge>& Graph::getEdges(Vertex vertex,
                                         Direction direction) const
{
  return (direction == Direction::OUTGOING) ?
    getOutgoing(vertex) :
    getIncoming(vertex);
}

bool Graph::contains(const Edge& edge) const
{
  for(const Edge& outEdge : getOutgoing(edge.getSource()))
  {
    if(outEdge == edge)
    {
      return true;
    }
  }

  return false;
}

Edge Graph::addEdge(Vertex source, Vertex target)
{
  assert((size_t) source.getIndex() < getVertices().size());
  assert((size_t) target.getIndex() < getVertices().size());

  Edge edge(source, target, edges.size());
  edges.push_back(edge);
  outgoing[source.getIndex()].push_back(edge);
  incoming[target.getIndex()].push_back(edge);

// const const  assert(check());

  return edge;
}

bool Graph::check() const
{
  for(const Vertex& vertex : getVertices())
  {
    for(const Edge& edge : getOutgoing(vertex))
    {
      assert(edge.getSource() == vertex);
    }

    for(const Edge& edge : getIncoming(vertex))
    {
      assert(edge.getTarget() == vertex);
    }
  }

  for(uint j = 0; j < getEdges().size(); ++j)
  {
    const Edge& edge = getEdges()[j];
    assert(edge.getIndex() == j);

    const Vertex& source = edge.getSource();
    const Vertex& target = edge.getTarget();

    bool found = false;

    for(const Edge& outEdge : getOutgoing(source))
    {
      if(outEdge == edge)
      {
        assert(outEdge.getSource() == source);
        assert(outEdge.getTarget() == target);
        assert(!found);
        found = true;
      }
    }

    assert(found);
    found = false;

    for(const Edge& inEdge : getIncoming(target))
    {
      if(inEdge == edge)
      {
        assert(inEdge.getSource() == source);
        assert(inEdge.getTarget() == target);
        assert(!found);
        found = true;
      }
    }

    assert(found);
  }

  return true;
}

VertexSet Graph::reachable(const Vertex& source) const
{
  VertexSet reachableVertices(*this);
  reachableVertices.insert(source);

  std::stack<Vertex> vertices;
  vertices.push(source);

  while(!vertices.empty())
  {
    Vertex current = vertices.top();
    vertices.pop();

    for(const Edge& edge : getOutgoing(current))
    {
      Vertex target = edge.getTarget();
      if(!reachableVertices.contains(target))
      {
        reachableVertices.insert(target);
        vertices.push(target);
      }
    }
  }

  return reachableVertices;
}


SubGraph Graph::inducedSubGraph(const std::vector<Vertex>& vertices) const
{
  SubGraph subGraph(*this, vertices);

  std::vector<Vertex> subVertices = ((const Graph&) subGraph).getVertices().collect();

  for(idx i = 0; i < vertices.size(); ++i)
  {
    const Vertex& source = vertices[i];

    for(const Edge& edge : getOutgoing(source))
    {
      for(idx j = 0; j < vertices.size(); ++j)
      {
        const Vertex& target = vertices[j];

        if(source == target)
        {
          continue;
        }

        if(edge.getTarget() == target)
        {
          subGraph.addEdge(subVertices[i], subVertices[j]);
        }
      }
    }
  }

  return subGraph;
}

SubGraph Graph::reachableSubGraph(const std::vector<Vertex>& vertices) const
{
  SubGraph subGraph(*this, vertices);

  std::vector<Vertex> subVertices = ((const Graph&) subGraph).getVertices().collect();

  for(idx i = 0; i < vertices.size(); ++i)
  {
    const Vertex& source = vertices[i];

    VertexSet reachableVertices = reachable(source);

    for(idx j = 0; j < vertices.size(); ++j)
    {
      const Vertex& target = vertices[j];

      if(source == target)
      {
        continue;
      }

      if(reachableVertices.contains(target))
      {
        subGraph.addEdge(subVertices[i], subVertices[j]);
      }
    }
  }

  return subGraph;
}


Graph Graph::complete(idx numVertices)
{
  Graph graph(numVertices, {});

  for(const Vertex& source : graph.getVertices())
  {
    for(const Vertex& target : graph.getVertices())
    {
      if(source == target)
      {
        continue;
      }

      graph.addEdge(source, target);
    }
  }

  return graph;
}
