#include "timed_edge.hh"

#include "graph/vertex_set.hh"

#include <cassert>

TimedEdge::TimedEdge(TimedVertex source,
                     TimedVertex target,
                     idx index)
  : source(source),
    target(target),
    index(index)
{
  if(source.getTime() > target.getTime())
  {
    throw std::invalid_argument("Edge arrives before leaving");
  }
}

TimedVertex TimedEdge::getSource() const
{
  return source;
}

TimedVertex TimedEdge::getTarget() const
{
  return target;
}

idx TimedEdge::getIndex() const
{
  return index;
}

idx TimedEdge::travelTime() const
{
  return target.getTime() - source.getTime();
}


bool TimedEdge::operator==(const TimedEdge& other) const
{
  return getSource() == other.getSource() &&
    getTarget() == other.getTarget() &&
    getIndex() == other.getIndex();
}

bool TimedEdge::operator!=(const TimedEdge& other) const
{
  return !(*this == other);
}

bool TimedEdge::enters(const VertexSet& vertices) const
{
  return !vertices.contains(getSource()) && vertices.contains(getTarget());
}

bool TimedEdge::leaves(const VertexSet& vertices) const
{
  return vertices.contains(getSource()) && !vertices.contains(getTarget());
}
