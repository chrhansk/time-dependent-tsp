#include "timed_path.hh"

#include <cassert>

#include <unordered_set>

#include "time_expanded_graph.hh"

TimedPath::TimedPath(std::initializer_list<TimedEdge> edges)
  : edges(new std::deque<TimedEdge>())
{
  for(auto it = edges.begin(); it != edges.end(); ++it)
  {
    append(*it);
  }
}

void TimedPath::append(const TimedEdge& edge)
{
  if(!edges->empty())
  {
    assert((*edges->rbegin()).getTarget() == edge.getSource());
  }

  edges->push_back(edge);
}

void TimedPath::prepend(const TimedEdge& edge)
{
  if(!edges->empty())
  {
    assert((*edges->begin()).getSource() == edge.getTarget());
  }

  edges->push_front(edge);
}

const std::deque<TimedEdge>& TimedPath::getEdges() const
{
  return *edges;
}

void TimedPath::add(const TimedEdge& edge, Direction direction)
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

idx TimedPath::travelTime() const
{
  idx travelTime = 0;

  for(const TimedEdge& timedEdge : getEdges())
  {
    travelTime += timedEdge.travelTime();
  }

  return travelTime;
}

bool TimedPath::connects(TimedVertex source, TimedVertex target) const
{
  if(getEdges().empty())
  {
    return source == target;
  }

  return getEdges().begin()->getSource() == source and
    getEdges().rbegin()->getTarget() == target;
}

bool TimedPath::connects(TimedVertex source,
                    TimedVertex target,
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

bool TimedPath::contains(TimedVertex vertex) const
{
  for(const TimedEdge& edge : getEdges())
  {
    if(edge.getSource() == vertex)
    {
      return true;
    }
  }

  return getEdges().rbegin()->getTarget() == vertex;
}

bool TimedPath::contains(TimedEdge edge) const
{
  return std::find(std::begin(getEdges()),
                   std::end(getEdges()),
                   edge) != std::end(getEdges());
}

bool TimedPath::isSimple() const
{
  if(getEdges().empty())
  {
    return true;
  }

  std::unordered_set<TimedVertex> vertices;

  for(const TimedEdge& edge : getEdges())
  {
    if(vertices.find(edge.getSource()) != vertices.end())
    {
      return false;
    }
  }

  return vertices.find(getEdges().rbegin()->getTarget())
    == vertices.end();
}

TimedVertex TimedPath::getSource() const
{
  assert(!getEdges().empty());

  return getEdges().begin()->getSource();
}

TimedVertex TimedPath::getTarget() const
{
  assert(!getEdges().empty());

  return getEdges().rbegin()->getTarget();
}

Path TimedPath::underlyingPath(const TimeExpandedGraph& graph) const
{
  Path path;

  for(const TimedEdge& edge : getEdges())
  {
    path.append(graph.underlyingEdge(edge));
  }

  return path;
}

idx TimedPath::girth(const TimeExpandedGraph& graph) const
{
  int minSize = inf;

  VertexMap<int> indices(graph.underlyingGraph(), -1);

  int currentIndex = 0;

  for(const TimedVertex& timedVertex : getVertices())
  {
    Vertex vertex = graph.underlyingVertex(timedVertex);

    if(indices(vertex) != -1)
    {
      minSize = std::min(minSize, currentIndex - indices(vertex));
    }

    indices(vertex) = currentIndex++;
  }

  return minSize;
}
