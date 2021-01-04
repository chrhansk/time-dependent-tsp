#include "min_arborescence.hh"

#include "graph/vertex_map.hh"

#include <queue>

EdgeSet contractCycle(const Graph& graph,
                      const Vertex& root,
                      const EdgeFunc<num>& weights,
                      const std::vector<Edge>& cycle)
{
  Graph subGraph;
  EdgeMap<num> subWeights(subGraph, 0);
  VertexMap<Vertex> subVertices(graph, Vertex());
  VertexMap<num> incomingWeights(graph, -inf);
  EdgeMap<Edge> originalEdges(subGraph, Edge());

  for(const Edge& edge : cycle)
  {
    incomingWeights(edge.getTarget()) = weights(edge);
  }

  auto cycleContains = [&](const Vertex& vertex) -> bool
    {
      return incomingWeights(vertex) != -inf;
    };

  auto cycleEdge = [&](const Edge& edge) -> bool
    {
      return cycleContains(edge.getSource()) &&
      cycleContains(edge.getTarget());
    };

  for(const Vertex& vertex : graph.getVertices())
  {
    if(cycleContains(vertex))
    {
      continue;
    }

    Vertex subVertex = subGraph.addVertex();

    subVertices(vertex) = subVertex;
  }

  Vertex cycleVertex = subGraph.addVertex();

  for(const Edge& edge : cycle)
  {
    subVertices(edge.getSource()) = cycleVertex;
  }

  for(const Edge& edge : graph.getEdges())
  {
    if(cycleEdge(edge))
    {
      continue;
    }

    num weight = weights(edge);

    Vertex source = edge.getSource();
    Vertex target = edge.getTarget();

    if(cycleContains(target))
    {
      weight -= incomingWeights(target);
    }

    Edge subEdge = subGraph.addEdge(subVertices(source),
                                    subVertices(target));

    originalEdges.extend(subEdge, edge);
    subWeights.extend(subEdge, weight);
  }

  Vertex subRoot = subVertices(root);

  EdgeSet subArborescence = minArborescence(subGraph,
                                            subRoot,
                                            subWeights.getValues());

  EdgeSet arborescence(graph);

  for(const Edge& subEdge : subGraph.getEdges())
  {
    if(subArborescence.contains(subEdge))
    {
      arborescence.insert(originalEdges(subEdge));
    }
  }

  Edge incomingEdge;
  bool found = false;

  for(const Edge& subEdge : subGraph.getIncoming(cycleVertex))
  {
    if(subArborescence.contains(subEdge))
    {
      incomingEdge = originalEdges(subEdge);
      found = true;
      break;
    }
  }

  assert(found);

  for(const Edge& edge : cycle)
  {
    if(edge.getTarget() == incomingEdge.getTarget())
    {
      continue;
    }

    arborescence.insert(edge);
  }

  return arborescence;
}

bool findCycle(const Graph& graph,
               const EdgeSet& arborescence,
               std::vector<Edge>& cycle)
{
  VertexMap<num> components(graph, -1);
  VertexMap<Edge> incoming(graph, Edge());

  num currentComponent = 0;

  for(const Vertex& root : graph.getVertices())
  {
    if(components(root) != -1)
    {
      continue;
    }

    std::queue<Vertex> vertices;

    vertices.push(root);
    components(root) = currentComponent;

    while(!vertices.empty())
    {
      Vertex current = vertices.front();
      vertices.pop();

      for(const Edge& outgoing : graph.getOutgoing(current))
      {
        if(!arborescence.contains(outgoing))
        {
          continue;
        }

        Vertex next = outgoing.getTarget();

        if(components(next) == components(current))
        {
          Vertex vertex = current;

          while(vertex != next)
          {
            const Edge& edge = incoming(vertex);
            cycle.push_back(edge);
            vertex = edge.getSource();
          }

          std::reverse(std::begin(cycle), std::end(cycle));

          cycle.push_back(outgoing);

          return true;
        }

        if(components(next) == -1)
        {
          incoming(next) = outgoing;
          components(next) = currentComponent;
          vertices.push(next);
        }
      }
    }

    ++currentComponent;
  }

  return false;
}

EdgeSet minArborescence(const Graph& graph,
                        const Vertex& root,
                        const EdgeFunc<num>& weights)
{
  EdgeSet arborescence(graph);

  for(const Vertex& vertex : graph.getVertices())
  {
    if(vertex == root)
    {
      continue;
    }

    if(graph.getIncoming(vertex).empty())
    {
      throw std::invalid_argument("Graph is not sufficiently connected");
    }

    num minWeight = inf;
    Edge minEdge;

    for(const Edge& edge : graph.getIncoming(vertex))
    {
      num weight = weights(edge);
      if(weight < minWeight)
      {
        minWeight = weight;
        minEdge = edge;
      }
    }

    arborescence.insert(minEdge);
  }

  std::vector<Edge> cycle;

  if(!findCycle(graph, arborescence, cycle))
  {
    return arborescence;
  }

  return contractCycle(graph, root, weights, cycle);
}
