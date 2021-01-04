#include "edge.hh"

#include "vertex_set.hh"

#include <cassert>

Vertex Edge::getSource() const
{
  return source;
}

Vertex Edge::getTarget() const
{
  return target;
}

Vertex Edge::getEndpoint(Direction direction) const
{
  return (direction == Direction::OUTGOING)
    ? getTarget()
    : getSource();
}

Vertex Edge::getOpposite(Vertex vertex) const
{
  if(vertex == getSource())
  {
    return getTarget();
  }
  else
  {
    assert(vertex == getTarget());
    return getSource();
  }
}

idx Edge::getIndex() const
{
  return index;
}

bool Edge::operator==(const Edge& other) const
{
  return getIndex() == other.getIndex();
}

bool Edge::operator!=(const Edge& other) const
{
  return !(*this == other);
}

bool Edge::enters(const VertexSet& vertices) const
{
  return !vertices.contains(getSource()) && vertices.contains(getTarget());
}

bool Edge::leaves(const VertexSet& vertices) const
{
  return vertices.contains(getSource()) && !vertices.contains(getTarget());
}

bool Edge::intersects(const VertexSet& vertices) const
{
  return vertices.contains(getSource()) || vertices.contains(getTarget());
}
