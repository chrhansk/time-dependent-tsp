#ifndef TIMED_ROUTER_HH
#define TIMED_ROUTER_HH

#include "router/router.hh"
#include "router/label_heap.hh"

#include "timed_edge_func.hh"
#include "timed_vertex_func.hh"

class TimedRouter
{
public:
  /**
   * Finds the shortest path between the given vertices
   * with respect to the given (non-negative) costs.
   *
   * @param source       The source Vertex
   * @param target       The target Vertex
   * @param costs        A function containing time-dependent (non-negative) costs.
   * @param startingTime The point in time at which the tour starts.
   *
   * @return A SearchResult
   **/
  virtual SearchResult<> shortestPath(Vertex source,
                                      Vertex target,
                                      const TimedEdgeFunc<num>& costs,
                                      idx departureTime = 0) = 0;

  /**
   * Returns the arrival time when departing from the given
   * source to the given target at the given departureTime,
   * assuming that the travel time is given by the provided
   * cost function.
   **/
  virtual num arrivalTime(Vertex source,
                          Vertex target,
                          const TimedEdgeFunc<num>& costs,
                          idx departureTime = 0)
  {
    return departureTime + travelTime(source, target, costs, departureTime);
  }

  /**
   * Returns the arrival time when traveling along the
   * route given by the provided Vertex range.
   **/
  template <class It>
  num arrivalTime(It first,
                  It last,
                  const TimedEdgeFunc<num>& costs,
                  idx departureTime = 0)
  {
    if(first == last)
    {
      return departureTime;
    }

    auto prev = first;
    auto curr = prev;
    ++curr;

    num currentTime = departureTime;

    for(; curr != last;++curr)
    {
      currentTime = arrivalTime(*prev, *curr, costs, currentTime);
      prev = curr;
    }

    return currentTime;
  }

  /**
   * Returns the travel time when departing from the given
   * source to the given target at the given departureTime,
   * assuming that the travel time is given by the provided
   * cost function.
   **/
  virtual num travelTime(Vertex source,
                         Vertex target,
                         const TimedEdgeFunc<num>& costs,
                         idx departureTime = 0)
  {
    auto result = shortestPath(source, target, costs, departureTime);

    if(!result.found)
    {
      throw std::invalid_argument("Graph is not strongly connected");
    }

    assert(result.cost >= 0);

    return result.cost;
  }

  virtual ~TimedRouter() {}
};

/**
 * A class which find shortest paths by performing a unidirectional
 * search from the source Vertex.
 **/
class TimedDijkstra : public TimedRouter
{
private:
  const Graph& graph;

public:
  TimedDijkstra(const Graph& graph)
    : graph(graph) {}

  ~TimedDijkstra() {}

  SearchResult<> shortestPath(Vertex source,
                              Vertex target,
                              const TimedEdgeFunc<num>& costs,
                              idx departureTime = 0) override;

  /**
   * A function returning a shortest path which is bounded
   * and which in addition satisfies the given filter.
   *
   * @tparam Filter  A filter given by a function mapping from Edge%s
   *                 to boolean values
   * @tparam bounded Whether or not to respect the given bound value.
   **/
  template<class Filter = AllEdgeFilter, bool bounded = false>
  SearchResult<> shortestPath(Vertex source,
                              Vertex target,
                              const TimedEdgeFunc<num>& costs,
                              idx departureTime = 0,
                              const Filter& filter = Filter(),
                              num bound = inf);
};


template<class Filter, bool bounded>
SearchResult<> TimedDijkstra::shortestPath(Vertex source,
                                           Vertex target,
                                           const TimedEdgeFunc<num>& costs,
                                           idx departureTime,
                                           const Filter& filter,
                                           num bound)
{
  LabelHeap<Label<>> heap(graph);
  int settled = 0, labeled = 0;
  bool found = false;

  heap.update(Label<>(source, Edge(), departureTime));

  while(!heap.isEmpty())
  {
    const Label<>& current = heap.extractMin();

    ++settled;

    if(bounded)
    {
      if(current.getCost() > bound)
      {
        break;
      }
    }

    if(current.getVertex() == target)
    {
      found = true;
      break;
    }

    for(const Edge& edge : graph.getOutgoing(current.getVertex()))
    {
      if(!filter(edge))
      {
        continue;
      }

      ++labeled;

      Label<> nextLabel = Label<>(edge.getTarget(),
                                  edge, current.getCost() + costs(edge, current.getCost()));

      heap.update(nextLabel);
    }

  }

  if(found)
  {
    Path path;

    Label<> current = heap.getLabel(target);
    const num cost = current.getCost();

    while(!(current.getVertex() == source))
    {
      Edge edge = current.getEdge();
      path.prepend(edge);
      current = heap.getLabel(edge.getSource());
    }

    return SearchResult<>(settled, labeled, true, path, cost - departureTime);
  }

  return SearchResult<>::notFound(settled, labeled);
}

class TimedShortestPathDistances : public TimedDistanceFunc
{
private:
  TimedRouter &router;
  const TimedEdgeFunc<num>& costs;

public:
  TimedShortestPathDistances(TimedRouter& router,
                             const TimedEdgeFunc<num>& costs)
    : router(router),
      costs(costs)
  {}

  num operator()(const Vertex& source,
                 const Vertex& target,
                 idx departureTime) override
  {
    auto result = router.shortestPath(source, target, costs, departureTime);

    if(!result.found)
    {
      throw std::invalid_argument("Graph is not strongly connected");
    }

    return result.cost;
  }
};

#endif /* TIMED_ROUTER_HH */
