#include "max_flow.hh"

#include "graph/vertex_map.hh"

#include <queue>

MaxFlowResult computeMaxFlow(const Graph& graph,
                             const Vertex& source,
                             const Vertex& target,
                             const EdgeFunc<double>& capacities)
{
  struct Entry
  {
    Edge edge;
    double value;
  };

  MaxFlowResult result(graph);
  EdgeMap<double> &flow = result.flow;

  while(true)
  {
    VertexSet visited(graph);
    std::queue<Vertex> vertices;

    VertexMap<Entry> entries(graph, Entry{Edge(), 0});

    entries(source) = Entry{Edge(), std::numeric_limits<double>::max()};
    vertices.push(source);
    visited.insert(source);

    while(!vertices.empty())
    {
      Vertex current = vertices.front();
      vertices.pop();

      if(current == target)
      {
        break;
      }

      for(const Edge& edge : graph.getOutgoing(current))
      {
        Vertex next = edge.getTarget();

        if(visited.contains(next))
        {
          continue;
        }

        double diff = capacities(edge) - flow(edge);

        if(diff > 0)
        {
          vertices.push(next);
          visited.insert(next);
          entries(next) = Entry{edge, std::min(entries(current).value, diff)};
        }
      }

      for(const Edge& edge : graph.getIncoming(current))
      {
        Vertex next = edge.getSource();

        if(visited.contains(next))
        {
          continue;
        }

        double diff = flow(edge);

        if(diff > 0)
        {
          vertices.push(next);
          visited.insert(next);
          entries(next) = Entry{edge, std::min(entries(current).value, diff)};
        }
      }
    }

    if(visited.contains(target))
    {
      Vertex current = target;

      const double value = entries(target).value;

      while(current != source)
      {
        Edge edge = entries(current).edge;

        if(edge.getTarget() == current)
        {
          flow(edge) = std::min(capacities(edge), flow(edge) + value);
          current = edge.getSource();
        }
        else
        {
          assert(edge.getSource() == current);
          assert(flow(edge) >= value);

          flow(edge) = std::min(0., flow(edge) - value);
          current = edge.getTarget();
        }
      }

      result.value += value;
    }
    else
    {
      result.cut = visited;
      return result;
    }

  }

  return result;
}

MinCutResult computeMinCut(const Graph& graph,
                           const EdgeFunc<double>& capacities)
{
  MinCutResult minCut(graph);

  std::vector<Vertex> vertices = graph.getVertices().collect();

  if(vertices.empty())
  {
    return minCut;
  }

  Vertex source = vertices.back();
  vertices.pop_back();

  for(const Vertex& target : vertices)
  {
    auto currentResult = computeMaxFlow(graph, source, target, capacities);

    if(currentResult.value < minCut.value)
    {
      minCut = MinCutResult(currentResult.cut, currentResult.value);
    }

    currentResult = computeMaxFlow(graph, target, source, capacities);

    if(currentResult.value < minCut.value)
    {
      minCut = MinCutResult(currentResult.cut, currentResult.value);
    }

  }

  return minCut;
}

MinCutResult computeMinCut(const Graph& graph,
                           const EdgeFunc<double>& capacities,
                           const Vertex& source)
{
  MinCutResult minCut(graph);

  std::vector<Vertex> vertices = graph.getVertices().collect();

  if(vertices.empty())
  {
    return minCut;
  }

  for(const Vertex& target : vertices)
  {
    if(source == target)
    {
      continue;
    }

    auto currentResult = computeMaxFlow(graph, source, target, capacities);

    if(currentResult.value < minCut.value)
    {
      minCut = MinCutResult(currentResult.cut, currentResult.value);
    }
  }

  return minCut;
}
