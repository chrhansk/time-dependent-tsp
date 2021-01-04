#include <gtest/gtest.h>

#include "arborescence/min_arborescence.hh"

TEST(Arborescence, testMinArborescence)
{
  Graph graph(6, {});

  std::vector<Vertex> vertices = graph.getVertices().collect();

  Vertex root = vertices.front();

  EdgeMap<num> weights(graph, 0);

  weights.extend(graph.addEdge(vertices[0], vertices[1]),
                 2);

  weights.extend(graph.addEdge(vertices[0], vertices[4]),
                 1);

  weights.extend(graph.addEdge(vertices[1], vertices[2]),
                 2);

  weights.extend(graph.addEdge(vertices[2], vertices[3]),
                 3);

  weights.extend(graph.addEdge(vertices[3], vertices[1]),
                 1);

  weights.extend(graph.addEdge(vertices[4], vertices[3]),
                 4);

  weights.extend(graph.addEdge(vertices[4], vertices[5]),
                 1);

  weights.extend(graph.addEdge(vertices[5], vertices[2]),
                 2);

  EdgeSet arborescence = minArborescence(graph,
                                         root,
                                         weights.getValues());

  double totalWeight = 0;

  for(const Edge& edge : graph.getEdges())
  {
    if(arborescence.contains(edge))
    {
      totalWeight += weights(edge);
    }
  }

  ASSERT_EQ(totalWeight, 8);
}
