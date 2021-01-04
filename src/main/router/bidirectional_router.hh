#ifndef BIDIRECTIONAL_ROUTER_HH
#define BIDIRECTIONAL_ROUTER_HH

#include "graph/graph.hh"
#include "graph/edge_map.hh"

#include "router/label.hh"
#include "router/label_heap.hh"
#include "router/router.hh"

/**
 * A base class for finding bidirectional shortest paths.
 **/
template <class T = num>
class BidirectionalRouter
{
protected:
  const Graph& graph;
public:
  BidirectionalRouter(const Graph& graph)
    : graph(graph)
  {}

  /**
   * Finds a shortest path between the given source and target Vertex
   * which satisfies the given filters and which is bounded by
   * the given bound value.
   *
   * @param source     The source Vertex
   * @param target     The target Vertex
   * @param costs      The (non-negative) costs
   * @param boundValue An upper bound on the path length. If a path
   *                   is returned it is guaranteed to have a length
   *                   of at most the given bound value.
   *
   * @tparam ForwardFilter  A filter given by a function mapping from Edge%s
   *                        to boolean values.
   * @tparam BackwardFilter A filter given by a function mapping from Edge%s
   *                        to boolean values.
   * @tparam bounded        Whether or not to respect the given bound value.
   *
   * @return A SearchResult
   **/
  template<class ForwardFiler = AllEdgeFilter,
           class BackwardFilter = AllEdgeFilter,
           bool bounded = false>
  SearchResult<T> shortestPath(Vertex source,
                               Vertex target,
                               const EdgeFunc<T>& costs,
                               ForwardFiler forwardFilter = ForwardFiler(),
                               BackwardFilter backwardFilter = BackwardFilter(),
                               const num boundValue = inf);
};

template<class T>
template<class ForwardFiler,
         class BackwardFilter,
         bool bounded>
SearchResult<T> BidirectionalRouter<T>::shortestPath(Vertex source,
                                                     Vertex target,
                                                     const EdgeFunc<T>& costs,
                                                     ForwardFiler forwardFilter,
                                                     BackwardFilter backwardFilter,
                                                     const num boundValue)
{
  int settled = 0, labeled = 0;
  bool found = false;

  Vertex split;
  T splitValue = inf;

  if(source == target)
  {
    return SearchResult<T>(0, 0, true, Path(), 0);
  }

  LabelHeap<Label<T>> forwardHeap(graph);
  LabelHeap<Label<T>> backwardHeap(graph);

  forwardHeap.update(Label<T>(source, Edge(), 0));
  backwardHeap.update(Label<T>(target, Edge(), 0));

  while(!(forwardHeap.isEmpty() or backwardHeap.isEmpty()))
  {
    const T bestValue = forwardHeap.peek().getCost()
      + backwardHeap.peek().getCost();

    if(bestValue >= splitValue)
    {
      break;
    }

    if(bounded and bestValue > boundValue)
    {
      break;
    }

    if(forwardHeap.peek().getCost() < backwardHeap.peek().getCost())
    {
      Label<T> current = forwardHeap.extractMin();

      ++settled;

      for(const Edge& edge : graph.getOutgoing(current.getVertex()))
      {
        if(!forwardFilter(edge))
        {
          continue;
        }

        Vertex nextVertex = edge.getTarget();
        T nextCost = current.getCost() + costs(edge);
        ++labeled;

        forwardHeap.update(Label<T>(nextVertex, edge, nextCost));

        const Label<T>& other = backwardHeap.getLabel(nextVertex);

        if(other.getState() != State::UNKNOWN)
        {
          T value = other.getCost() + nextCost;

          if(value < splitValue)
          {
            splitValue = value;
            split = nextVertex;
            found = true;
          }
        }
      }
    }
    else
    {
      Label<T> current = backwardHeap.extractMin();

      ++settled;

      for(const Edge& edge : graph.getIncoming(current.getVertex()))
      {
        if(!backwardFilter(edge))
        {
          continue;
        }

        Vertex nextVertex = edge.getSource();
        T nextCost = current.getCost() + costs(edge);

        ++labeled;

        backwardHeap.update(Label<T>(nextVertex, edge, nextCost));

        const Label<T>& other = forwardHeap.getLabel(nextVertex);

        if(other.getState() != State::UNKNOWN)
        {
          T value = other.getCost() + nextCost;

          if(value < splitValue)
          {
            splitValue = value;
            split = nextVertex;
            found = true;
          }
        }
      }
    }
  }

  if(found)
  {
    if(bounded)
    {
      if(splitValue > boundValue)
      {
        return SearchResult<T>::notFound(settled, labeled);
      }
    }

    Path path;

    Label<T> current = forwardHeap.getLabel(split);

    while(!(current.getVertex() == source))
    {
      Edge edge = current.getEdge();
      assert(forwardFilter(edge));
      path.prepend(edge);
      current = forwardHeap.getLabel(edge.getSource());
    }

    current = backwardHeap.getLabel(split);

    while(!(current.getVertex() == target))
    {
      Edge edge = current.getEdge();
      assert(backwardFilter(edge));
      path.append(edge);
      current = backwardHeap.getLabel(edge.getTarget());
    }

    assert(path.connects(source, target));
    assert(path.cost(costs) == splitValue);

    return SearchResult<T>(settled, labeled, true, path, splitValue);
  }

  return SearchResult<T>::notFound(settled, labeled);
}


/**
 * A class which finds a shortest path by performing a bidirectional
 * search between a source and target Vertex.
 **/
template<class T = num>
class BidirectionalDijkstra : public Router<T>,
                              protected BidirectionalRouter<T>
{
public:
  BidirectionalDijkstra(const Graph& graph)
    : BidirectionalRouter<T>(graph)
  {}

  SearchResult<T> shortestPath(Vertex source,
                               Vertex target,
                               const EdgeFunc<T>& costs) override
  {
    return BidirectionalRouter<T>::template shortestPath<AllEdgeFilter,
                                                         AllEdgeFilter,
                                                         false>(source,
                                                                target,
                                                                costs,
                                                                AllEdgeFilter(),
                                                                AllEdgeFilter());
  }

  SearchResult<T> shortestPath(Vertex source,
                               Vertex target,
                               const EdgeFunc<T>& costs,
                               num bound) override
  {
    return BidirectionalRouter<T>::template shortestPath<AllEdgeFilter,
                                                         AllEdgeFilter,
                                                         true>(source,
                                                               target,
                                                               costs,
                                                               AllEdgeFilter(),
                                                               AllEdgeFilter(),
                                                               bound);
  }
};

#endif /* BIDIRECTIONAL_ROUTER_HH */
