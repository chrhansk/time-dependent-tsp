#include "tour.hh"

#include <algorithm>

Tour::Tour(const Graph& graph,
           const std::vector<Vertex>& vertices)
  : graph(graph),
    vertices(vertices)
{
  assert(!vertices.empty());
}

Tour::Tour(const Graph& graph, std::initializer_list<Vertex> vertexList)
  : graph(graph)
{
  vertices.insert(std::end(vertices), std::begin(vertexList), std::end(vertexList));
}

Tour::Tour(const Graph& graph,
           std::vector<Vertex>&& vertices)
  : graph(graph),
    vertices(vertices)
{
  assert(!vertices.empty());
}

bool Tour::contains(const Vertex& source, const Vertex& target) const
{
  for(idx i = 0; i < vertices.size() - 1; ++i)
  {
    if(vertices[i] == source && vertices[i + 1] == target)
    {
      return true;
    }
  }

  if(vertices.back() == source &&
     vertices.front() == target)
  {
    return true;
  }

  return false;
}

Path Tour::asPath() const
{
  auto last = std::begin(vertices);
  auto current = last;
  ++current;

  Path path;

  for(; current != std::end(vertices); ++current, ++last)
  {
    bool found = false;

    for(const Edge& outgoing : graph.getOutgoing(*last))
    {
      if(outgoing.getTarget() == *current)
      {
        path.append(outgoing);
        found = true;
      }
    }

    assert(found);
  }

  bool found = false;

  for(const Edge& outgoing : graph.getOutgoing(*last))
  {
    if(outgoing.getTarget() == getSource())
    {
      path.append(outgoing);
      found = true;
    }
  }

  assert(found);

  return path;
}

Tour& Tour::operator=(const Tour& other)
{
  if(&graph != &(other.graph))
  {
    throw std::invalid_argument("Attempt to assign tour belonging to different graph");
  }

  vertices = other.vertices;

  return *this;
}

num Tour::cost(DistanceFunc& distances) const
{
  if(vertices.size() <= 1)
  {
    return 0;
  }

  num totalCost = 0;

  for(auto it = vertices.begin(); it != vertices.end(); ++it)
  {
    auto next = it;
    ++next;

    Vertex currentVertex = *it;
    Vertex nextVertex = *(vertices.begin());

    if(next != vertices.end())
    {
      nextVertex = *next;
    }

    totalCost += distances(currentVertex, nextVertex);
  }

  return totalCost;
}

num Tour::cost(const TimedEdgeFunc<num>& costs, idx startingTime) const
{
  TimedDijkstra dijkstra(graph);

  return cost(costs, startingTime, dijkstra);
}

num Tour::cost(const TimedEdgeFunc<num>& costs,
               idx startingTime,
               TimedRouter& router) const
{
  if(vertices.size() <= 1)
  {
    return 0;
  }

  num totalCost = 0;
  idx currentTime = startingTime;

  for(auto it = vertices.begin(); it != vertices.end(); ++it)
  {
    auto next = it;
    ++next;

    Vertex currentVertex = *it;
    Vertex nextVertex = *(vertices.begin());

    if(next != vertices.end())
    {
      nextVertex = *next;
    }

    auto result = router.shortestPath(currentVertex,
                                      nextVertex,
                                      costs,
                                      currentTime);

    assert(result.found);

    num currentCost = result.path.cost(costs, currentTime);

    currentTime += currentCost;
    totalCost += currentCost;
  }

  return totalCost;
}

num Tour::cost(TimedDistanceFunc& distances,
               idx departureTime) const
{
  if(vertices.size() <= 1)
  {
    return 0;
  }

  num totalCost = 0;
  idx currentTime = departureTime;

  for(auto it = vertices.begin(); it != vertices.end(); ++it)
  {
    auto next = it;
    ++next;

    Vertex currentVertex = *it;
    Vertex nextVertex = *(vertices.begin());

    if(next != vertices.end())
    {
      nextVertex = *next;
    }

    num currentCost = distances(currentVertex, nextVertex, currentTime);

    currentTime += currentCost;
    totalCost += currentCost;
  }

  return totalCost;
}

bool Tour::connects(const std::vector<Vertex>& otherVertices) const
{
  if(vertices.size() != otherVertices.size())
  {
    return false;
  }

  std::vector<Vertex> sortedVertices(vertices);
  std::vector<Vertex> otherSortedVertices(otherVertices);

  std::sort(sortedVertices.begin(),
            sortedVertices.end());

  std::sort(otherSortedVertices.begin(),
            otherSortedVertices.end());

  return sortedVertices == otherSortedVertices;
}

void Tour::swapToBegin(const Vertex& source)
{
  auto pos = std::find(vertices.begin(),
                       vertices.end(),
                       source);

  if(pos == vertices.end())
  {
    throw std::invalid_argument("Vertex is not contained in tour");
  }

  if(pos == vertices.begin())
  {
    return;
  }

  std::vector<Vertex> nextVertices;

  for(auto it = pos; it != vertices.end(); ++it)
  {
    nextVertices.push_back(*it);
  }

  for(auto it = vertices.begin(); it != pos; ++it)
  {
    nextVertices.push_back(*it);
  }

  vertices = nextVertices;
}


std::ostream& operator<<(std::ostream& out, const Tour& tour)
{
  out << "[";

  for(const Vertex& vertex : tour.getVertices())
  {
    out << vertex.getIndex() << ", ";
  }

  out << "]";
  return out;
}
