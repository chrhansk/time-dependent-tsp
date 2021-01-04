#include "odd_cycle.hh"

#include "graph/vertex_map.hh"
#include "router/router.hh"

static double cutoff = 1e-5;

std::vector<std::vector<Vertex>>
findOddCycles(const Graph& graph,
              const EdgeFunc<double>& weights,
              double maxWeight)
{
  std::vector<std::vector<Vertex>> cycles;

  Graph cycleGraph(0, {});
  EdgeMap<double> cycleWeights(cycleGraph, 0);

  VertexMap<Vertex> leftVertices(graph, Vertex());
  VertexMap<Vertex> rightVertices(graph, Vertex());

  VertexMap<Vertex> originalVertices(cycleGraph, Vertex());

  for(const Vertex& vertex : graph.getVertices())
  {
    Vertex left = cycleGraph.addVertex();
    leftVertices(vertex) = left;

    originalVertices.addVertex(left, vertex);

    Vertex right = cycleGraph.addVertex();
    rightVertices(vertex) = right;

    originalVertices.addVertex(right, vertex);
  }

  for(const Edge& edge : graph.getEdges())
  {
    const double weight = weights(edge);
    Vertex source = edge.getSource();
    Vertex target = edge.getTarget();

    cycleWeights.extend(cycleGraph.addEdge(leftVertices(source),
                                           rightVertices(target)),
                        weight);

    cycleWeights.extend(cycleGraph.addEdge(rightVertices(target),
                                           leftVertices(source)),
                        weight);

    cycleWeights.extend(cycleGraph.addEdge(rightVertices(source),
                                           leftVertices(target)),
                        weight);

    cycleWeights.extend(cycleGraph.addEdge(leftVertices(target),
                                           rightVertices(source)),
                        weight);
  }

  VertexSet forbiddenVertices(cycleGraph);

  for(const Edge& edge : graph.getEdges())
  {
    Dijkstra<double> dijkstra(cycleGraph);

    auto result = dijkstra.shortestPath(leftVertices(edge.getSource()),
                                        leftVertices(edge.getTarget()),
                                        cycleWeights.getValues(),
                                        [&](const Edge& edge) -> bool
                                        {
                                          return !edge.intersects(forbiddenVertices);
                                        });

    if(!result.found)
    {
      continue;
    }

    const double weight = result.cost + weights(edge);

    if(weight < maxWeight - cutoff)
    {
      // found odd cycle...

      std::vector<Vertex> incompatCycle;

      for(const Vertex& vertex : result.path.getVertices())
      {
        Vertex incompatVertex = originalVertices(vertex);
        incompatCycle.push_back(incompatVertex);
      }

      VertexSet incompatSet(graph);

      bool isSimple = true;

      for(const Vertex& vertex : incompatCycle)
      {
        if(incompatSet.contains(vertex))
        {
          isSimple = false;
          break;
        }

        incompatSet.insert(vertex);
      }

      if(!isSimple)
      {
        continue;
      }

      assert((incompatCycle.size() % 2) == 1);

      // remove the edge further considerations
      {
        forbiddenVertices.insert(leftVertices(edge.getSource()));
        forbiddenVertices.insert(leftVertices(edge.getTarget()));

        forbiddenVertices.insert(rightVertices(edge.getSource()));
        forbiddenVertices.insert(rightVertices(edge.getTarget()));
      }

      cycles.push_back(std::move(incompatCycle));
    }
  }

  return cycles;
}
