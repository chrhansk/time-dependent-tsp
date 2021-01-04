#include "time_expanded_graph.hh"

TimeExpandedGraph::TimeExpandedGraph(const Graph& underlyingGraph)
  : graph(0, {}),
    underlying(underlyingGraph),
    vertexTimes(underlying, std::map<idx, Vertex>()),
    underlyingVertices(*this, Vertex()),
    underlyingEdges(*this, Edge()),
    timedEdges(underlying, std::vector<TimedEdge>{}),
    times(*this, 0)
{
}

TimedVertex TimeExpandedGraph::addVertex(Vertex vertex, idx time)
{
  std::map<idx, Vertex>& currentMap = vertexTimes(vertex);

  auto it = currentMap.find(time);

  assert(it == currentMap.end());

  Vertex nextVertex = graph.addVertex();

  currentMap.insert(std::make_pair(time, nextVertex));

  underlyingVertices.addVertex(nextVertex, vertex);
  times.addVertex(nextVertex, time);

  return TimedVertex(nextVertex.getIndex(), time);
}

const std::vector<TimedVertex>& TimeExpandedGraph::getTopologicalOrdering() const
{
  if(topologicalOrdering.size() != getVertices().size())
  {
    topologicalOrdering = getVertices().collect();

    std::sort(std::begin(topologicalOrdering), std::end(topologicalOrdering),
              [](const TimedVertex& first, const TimedVertex& second) -> bool
              {
                return first.getTime() < second.getTime();
              });
  }

  return topologicalOrdering;
}

TimedEdge TimeExpandedGraph::addEdge(TimedVertex source,
                                     TimedVertex target,
                                     Edge underlyingEdge)
{
  assert(underlyingVertex(source) == underlyingEdge.getSource());
  assert(underlyingVertex(target) == underlyingEdge.getTarget());

  Edge edge = graph.addEdge(Vertex(source.getIndex()), Vertex(target.getIndex()));

  underlyingEdges.extend(edge, underlyingEdge);

  TimedEdge timedEdge(source, target, edge.getIndex());

  timedEdges(underlyingEdge).push_back(timedEdge);

  return timedEdge;
}

TimedEdge TimeExpandedGraph::getEdge(Edge edge, idx time) const
{
  for(const TimedEdge& timedEdge : getOutgoing(getVertex(edge.getSource(), time)))
  {
    if(underlyingEdge(timedEdge) == edge)
    {
      return timedEdge;
    }
  }

  throw std::invalid_argument("Edge does not exist at the given time");
}

TimedVertex TimeExpandedGraph::getVertex(Vertex vertex, idx time) const
{
  const std::map<idx, Vertex>& currentMap = vertexTimes(vertex);

  auto it = currentMap.find(time);

  if(it != currentMap.end())
  {
    return TimedVertex(it->second.getIndex(), time);
  }
  else
  {
    throw std::invalid_argument("Vertex does not exist at the given time");
  }
}

TimedVertex TimeExpandedGraph::getVertex(Vertex vertex, idx time, bool add)
{
  std::map<idx, Vertex>& currentMap = vertexTimes(vertex);

  auto it = currentMap.find(time);

  if(it != currentMap.end())
  {
    return TimedVertex(it->second.getIndex(), time);
  }
  else
  {
    if(!add)
    {
      throw std::invalid_argument("Vertex does not exist at the given time");
    }
    else
    {
      return addVertex(vertex, time);
    }
  }
}

TimedVertex TimeExpandedGraph::asTimed(Vertex vertex) const
{
  return TimedVertex(vertex.getIndex(), times(vertex));
}


const std::map<idx, Vertex>&
TimeExpandedGraph::getVertexTimes(Vertex underlyingVertex) const
{
  return vertexTimes(underlyingVertex);
}

const std::vector<TimedVertex> TimeExpandedGraph::getExpandedVertices(Vertex underlyingVertex) const
{
  const std::map<idx, Vertex>& times = vertexTimes(underlyingVertex);
  std::vector<TimedVertex> timedVertices;
  timedVertices.reserve(times.size());

  for(const std::pair<idx, Vertex>& pair : times)
  {
    timedVertices.push_back(TimedVertex(pair.second.getIndex(), pair.first));
  }

  return timedVertices;
}

TimedPath TimeExpandedGraph::expandPath(const Path& path) const
{
  idx currentTime = 0;

  TimedPath timedPath;

  for(const Edge& edge : path.getEdges())
  {
    TimedEdge timedEdge = getEdge(edge, currentTime);

    timedPath.append(timedEdge);

    currentTime = timedEdge.getTarget().getTime();
  }

  return timedPath;
}

bool TimeExpandedGraph::hasEdge(const Edge& edge, idx time) const
{
  const std::map<idx, Vertex>& currentMap = vertexTimes(edge.getSource());

  auto it = currentMap.find(time);

  if(it == std::end(currentMap))
  {
    return false;
  }

  TimedVertex timedVertex(it->second.getIndex(), it->first);

  for(const TimedEdge& timedEdge : getOutgoing(timedVertex))
  {
    if(underlyingEdge(timedEdge) == edge)
    {
      return true;
    }
  }

  return false;
}

bool TimeExpandedGraph::hasVertex(Vertex vertex, idx time) const
{
  const std::map<idx, Vertex>& currentMap = vertexTimes(vertex);

  return currentMap.find(time) != currentMap.end();
}

Vertex TimeExpandedGraph::underlyingVertex(TimedVertex timedVertex) const
{
  return underlyingVertices(Vertex(timedVertex.getIndex()));
}

Vertex TimeExpandedGraph::underlyingVertex(Vertex vertex) const
{
  return underlyingVertices(vertex);
}

Edge TimeExpandedGraph::underlyingEdge(TimedEdge timedEdge) const
{
  return underlyingEdges(timedEdge);
}

Edge TimeExpandedGraph::underlyingEdge(Edge edge) const
{
  return underlyingEdges(edge);
}

SimpleEdgeFunc<num> TimeExpandedGraph::travelTimes() const
{
  return SimpleEdgeFunc<num>([this](const Edge& edge) -> num
                             {
                               return times(edge.getTarget()) - times(edge.getSource());
                             });
}
