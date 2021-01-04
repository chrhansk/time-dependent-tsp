#include "dijkstra_rank.hh"

#include "label.hh"
#include "label_heap.hh"

std::vector<Vertex> nearestVertices(const Graph& graph,
                                    Vertex source,
                                    const EdgeFunc<num>& costs,
                                    int size)
{
  std::vector<Vertex> vertices;

  LabelHeap<SimpleLabel> heap(graph);

  heap.update(SimpleLabel(source, 0));

  while(!heap.isEmpty())
  {
    const SimpleLabel& current = heap.extractMin();

    vertices.push_back(current.getVertex());

    if(size >= 0)
    {
      if(vertices.size() == (size_t) size)
      {
        break;
      }
    }

    for(const Edge& edge : graph.getOutgoing(current.getVertex()))
    {
      SimpleLabel nextLabel = SimpleLabel(edge.getTarget(),
                                          current.getCost() + costs(edge));

      heap.update(nextLabel);
    }

  }

  return vertices;
}

VertexMap<idx> dijkstraRanks(const Graph& graph,
                             Vertex source,
                             const EdgeFunc<num>& costs)
{
  VertexMap<idx> rankMap(graph, inf);
  idx currentRank = 0;
  LabelHeap<SimpleLabel> heap(graph);

  heap.update(SimpleLabel(source, 0));

  while(!heap.isEmpty())
  {
    const SimpleLabel& current = heap.extractMin();

    rankMap(current.getVertex()) = currentRank++;

    for(const Edge& edge : graph.getOutgoing(current.getVertex()))
    {
      SimpleLabel nextLabel = SimpleLabel(edge.getTarget(),
                                          current.getCost() + costs(edge));

      heap.update(nextLabel);
    }

  }

  return rankMap;
}
