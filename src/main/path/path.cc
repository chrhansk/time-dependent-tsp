#include "path.hh"

#include <cassert>

#include <unordered_set>

#include "graph/vertex_set.hh"

Path::Path(std::initializer_list<Edge> edges)
  : edges(new std::deque<Edge>())
{
  for(auto it = edges.begin(); it != edges.end(); ++it)
  {
    append(*it);
  }
}

void Path::append(const Edge& edge)
{
  if(!edges->empty())
  {
    assert((*edges->rbegin()).getTarget() == edge.getSource());
  }

  edges->push_back(edge);
}

void Path::prepend(const Edge& edge)
{
  if(!edges->empty())
  {
    assert((*edges->begin()).getSource() == edge.getTarget());
  }

  edges->push_front(edge);
}

const std::deque<Edge>& Path::getEdges() const
{
  return *edges;
}

void Path::add(const Edge& edge, Direction direction)
{
  if(direction == Direction::OUTGOING)
  {
    append(edge);
  }
  else
  {
    prepend(edge);
  }
}

bool Path::connects(Vertex source, Vertex target) const
{
  if(getEdges().empty())
  {
    return source == target;
  }

  return getEdges().begin()->getSource() == source and
    getEdges().rbegin()->getTarget() == target;
}

bool Path::connects(Vertex source,
                    Vertex target,
                    Direction direction) const
{
  if(direction == Direction::OUTGOING)
  {
    return connects(source, target);
  }
  else
  {
    return connects(target, source);
  }
}

bool Path::contains(Vertex vertex) const
{
  for(const Edge& edge : getEdges())
  {
    if(edge.getSource() == vertex)
    {
      return true;
    }
  }

  return getEdges().rbegin()->getTarget() == vertex;
}

bool Path::contains(Edge edge) const
{
  return std::find(std::begin(getEdges()),
                   std::end(getEdges()),
                   edge) != std::end(getEdges());
}

bool Path::isSimple() const
{
  if(getEdges().empty())
  {
    return true;
  }

  std::unordered_set<Vertex> vertices;

  for(const Edge& edge : getEdges())
  {
    if(vertices.find(edge.getSource()) != vertices.end())
    {
      return false;
    }
  }

  return vertices.find(getEdges().rbegin()->getTarget())
    == vertices.end();
}

Vertex Path::getSource() const
{
  assert(!getEdges().empty());

  return getEdges().begin()->getSource();
}

Vertex Path::getTarget() const
{
  assert(!getEdges().empty());

  return getEdges().rbegin()->getTarget();
}

bool Path::isTour(const Graph& graph) const
{
  if(getEdges().size() != graph.getVertices().size())
  {
    return false;
  }

  VertexSet vertices(graph);

  for(const Edge& edge : getEdges())
  {
    if(vertices.contains(edge.getSource()))
    {
      return false;
    }

    vertices.insert(edge.getSource());
  }

  return true;
}

Vertex Path::getEndpoint(Direction direction) const
{
  assert(!getEdges().empty());

  if(direction == Direction::OUTGOING)
  {
    return getTarget();
  }
  else
  {
    return getSource();
  }
}
