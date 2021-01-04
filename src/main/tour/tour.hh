#ifndef TOUR_HH
#define TOUR_HH

#include <iostream>

#include "graph/graph.hh"
#include "graph/edge_map.hh"

#include "router/router.hh"

#include "timed/timed_edge_func.hh"
#include "timed/timed_router.hh"

class Tour
{
private:
  const Graph& graph;
  std::vector<Vertex> vertices;
public:
  Tour(const Tour& other) = default;

  Tour(const Graph& graph, std::initializer_list<Vertex> vertices);

  Tour(const Graph& graph, const std::vector<Vertex>& vertices);

  Tour(const Graph& graph, std::vector<Vertex>&& vertices);

  Tour& operator=(const Tour&);

  template<class T>
  T cost(const EdgeFunc<T>& costs) const;

  template<class T>
  T cost(const EdgeFunc<T>& costs,
         Router<T>& router) const;

  num cost(DistanceFunc& distances) const;

  num cost(const TimedEdgeFunc<num>& costs,
           idx departureTime = 0) const;

  num cost(const TimedEdgeFunc<num>& costs,
           idx departureTime,
           TimedRouter& router) const;

  num cost(TimedDistanceFunc& distances,
           idx departureTime = 0) const;

  bool contains(const Vertex& source, const Vertex& target) const;

  const std::vector<Vertex>& getVertices() const
  {
    return vertices;
  }

  const Graph& getGraph() const
  {
    return graph;
  }

  Vertex getSource() const
  {
    return *(vertices.begin());
  }

  Path asPath() const;

  std::vector<Vertex>& getVertices()
  {
    return vertices;
  }

  bool beginsAt(const Vertex& vertex) const
  {
    return *(vertices.begin()) == vertex;
  }

  bool connects(const std::vector<Vertex>& otherVertices) const;

  void swapToBegin(const Vertex& source);
};

std::ostream& operator<<(std::ostream& out, const Tour& tour);

template<class T = num>
class TourEvaluator
{
public:
  virtual T operator()(const Tour& tour) const = 0;
};

template<class T = num>
class DistanceEvaluator : public TourEvaluator<T>
{
private:
  Router<T>& router;
  const EdgeFunc<T>& costs;

public:
  DistanceEvaluator(Router<T>& router,
                    const EdgeFunc<T>& costs)
    : router(router),
      costs(costs)
  {}

  T operator()(const Tour& tour) const override
  {
    return tour.cost(costs, router);
  }
};

class TimedDistanceEvaluator : public TourEvaluator<num>
{
private:
  TimedDistanceFunc& distances;

public:
  TimedDistanceEvaluator(TimedDistanceFunc& distances)
    : distances(distances)
  {}

  num operator()(const Tour& tour) const override
  {
    return tour.cost(distances);
  }
};


template<class T>
T Tour::cost(const EdgeFunc<T>& costs,
               Router<T>& router) const
{
  if(vertices.size() <= 1)
  {
    return 0;
  }

  T s = 0;

  Vertex last = *(vertices.rbegin());

  for(const Vertex& vertex: vertices)
  {
    auto result = router.shortestPath(last, vertex, costs);

    if(!result.found)
    {
      throw std::runtime_error("Graph is not strongly connected");
    }

    s += result.path.cost(costs);

    last = vertex;
  }

  return s;
}

template<class T>
T Tour::cost(const EdgeFunc<T>& costs) const
{
  Dijkstra<T> dijkstra(graph);

  return cost(costs, dijkstra);
}


#endif /* TOUR_HH */
