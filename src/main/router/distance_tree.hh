#ifndef DISTANCE_TREE_HH
#define DISTANCE_TREE_HH

#include <stdexcept>

#include "graph/graph.hh"
#include "graph/edge_map.hh"

#include "label.hh"
#include "label_heap.hh"
#include "router.hh"

template <Direction direction = Direction::OUTGOING, class T = num>
class DistanceTree
{
private:
  const Graph& graph;
  const EdgeFunc<T>& costs;

  LabelHeap<SimpleLabel<T>> heap;
  T maxDist;

  template<class Predicate, class Filter = AllEdgeFilter>
  void extendWhile(Predicate predicate, Filter filter = Filter());

public:
  DistanceTree(const Graph& graph,
               const EdgeFunc<T>& costs);

  DistanceTree(const Graph& graph,
               const EdgeFunc<T>& costs,
               Vertex root);

  template <class It>
  DistanceTree(const Graph& graph,
               const EdgeFunc<T>& costs,
               It begin,
               It end);

  template<class Filter = AllEdgeFilter>
  void extend(Filter filter = Filter());

  template<class Filter = AllEdgeFilter>
  Vertex next(Filter filter = Filter());

  template<class Filter = AllEdgeFilter>
  void extend(Vertex vertex, Filter filter = Filter());

  template <class It, class Filter = AllEdgeFilter>
  void extend(It begin, It end, Filter filter = Filter());

  bool explored(Vertex vertex) const;

  T distance(Vertex vertex) const;

  void add(Vertex vertex, T distance = 0);

  bool done() const
  {
    return heap.isEmpty();
  }

  T maxDistance() const
  {
    return maxDist;
  }

};

template <Direction direction, class T>
DistanceTree<direction, T>::DistanceTree(const Graph& graph,
                                         const EdgeFunc<T>& costs)
  : graph(graph),
    costs(costs),
    heap(graph),
    maxDist(0)
{
}

template <Direction direction, class T>
DistanceTree<direction, T>::DistanceTree(const Graph& graph,
                                         const EdgeFunc<T>& costs,
                                         Vertex root)
  : graph(graph),
    costs(costs),
    heap(graph),
    maxDist(0)
{
  add(root);
}

template <Direction direction, class T>
template <class It>
DistanceTree<direction, T>::DistanceTree(const Graph& graph,
                                         const EdgeFunc<T>& costs,
                                         It begin,
                                         It end)
  : graph(graph),
    costs(costs),
    heap(graph)
{
  for(auto it = begin; it != end; ++it)
  {
    add(*it);
  }
}

template <Direction direction, class T>
template <class Predicate, class Filter>
void DistanceTree<direction, T>::extendWhile(Predicate predicate, Filter filter)
{
  while(!heap.isEmpty() and predicate())
  {
    next(filter);
  }
}

template <Direction direction, class T>
template <class Filter>
Vertex DistanceTree<direction, T>::next(Filter filter)
{
  assert(!done());

  const SimpleLabel<T>& current = heap.extractMin();

  maxDist = std::max(maxDist, current.getCost());

  for(const Edge& edge : graph.getEdges(current.getVertex(), direction))
  {
    if(!filter(edge))
    {
      continue;
    }

    SimpleLabel<T> nextLabel = SimpleLabel<T>(edge.getEndpoint(direction),
                                              current.getCost() + costs(edge));

    heap.update(nextLabel);
  }

  return current.getVertex();
}

template <Direction direction, class T>
template <class Filter>
void DistanceTree<direction, T>::extend(Filter filter)
{
  extendWhile([] () -> bool {return true;}, filter);
}

template<Direction direction, class T>
template<class Filter>
void DistanceTree<direction, T>::extend(Vertex vertex, Filter filter)
{
  extendWhile([&] () -> bool {return !explored(vertex);}, filter);
}

template <Direction direction, class T>
template <class It, class Filter>
void DistanceTree<direction, T>::extend(It begin, It end, Filter filter)
{
  for(auto it = begin; it != end; ++it)
  {
    extendWhile([&] () -> bool {return !explored(*it);}, filter);
  }
}

template <Direction direction, class T>
bool DistanceTree<direction, T>::explored(Vertex vertex) const
{
  return heap.getLabel(vertex).getState() == State::SETTLED;
}

template <Direction direction, class T>
T DistanceTree<direction, T>::distance(Vertex vertex) const
{
  if(!explored(vertex))
  {
    throw std::invalid_argument("Vertex has not been explored");
  }

  return heap.getLabel(vertex).getCost();
}

template <Direction direction, class T>
void DistanceTree<direction, T>::add(Vertex vertex, T distance)
{
  if(explored(vertex))
  {
    throw std::invalid_argument("Vertex has already been explored");
  }

  maxDist = std::max(maxDist, distance);

  return heap.update(SimpleLabel<T>(vertex, distance));
}

#endif /* DISTANCE_TREE_HH */
