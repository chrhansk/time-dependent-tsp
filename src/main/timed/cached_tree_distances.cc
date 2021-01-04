#include "cached_tree_distances.hh"

#include "router/label.hh"
#include "router/label_heap.hh"

num CachedTreeDistances::operator()(const Vertex& source,
                                    const Vertex& target,
                                    idx departureTime)
{
  return cache.get(Entry{source, target, departureTime});
}

num CachedTreeDistances::shortestPath(const Entry& entry)
{
  const Vertex& source = entry.source;
  const Vertex& target = entry.target;
  const idx departureTime = entry.departureTime;

  LabelHeap<Label<>> heap(graph);
  int settled = 0, labeled = 0;
  bool found = false;

  heap.update(Label<>(source, Edge(), departureTime));

  while(!heap.isEmpty())
  {
    const Label<>& current = heap.extractMin();

    ++settled;

    if(current.getVertex() == target)
    {
      found = true;
      break;
    }

    for(const Edge& edge : graph.getOutgoing(current.getVertex()))
    {
      ++labeled;

      Label<> nextLabel = Label<>(edge.getTarget(),
                                  edge, current.getCost() + costs(edge, current.getCost()));

      heap.update(nextLabel);
    }
  }

  for(const Vertex& vertex : vertices)
  {
    Label<> vertexLabel = heap.getLabel(vertex);
    if(vertexLabel.getState() == State::SETTLED)
    {
      cache.insert(Entry{source, vertex, departureTime}, vertexLabel.getCost() - departureTime);
    }
  }

  if(!found)
  {
    throw std::invalid_argument("Graph is not strongly connected");
  }

  Label<> targetLabel = heap.getLabel(target);
  return targetLabel.getCost() - departureTime;
}
