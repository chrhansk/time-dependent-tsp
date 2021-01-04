#include "unitary_afc_separator.hh"

#include "flow/max_flow.hh"

UnitaryAFCSeparator::UnitaryAFCSeparator(const TimeExpandedGraph& graph,
                                         const Vertex& originalSource)
  : graph(graph),
    originalGraph(graph.underlyingGraph()),
    originalSource(originalSource)
{

}

std::vector<UnitaryAFCSet>
UnitaryAFCSeparator::separate(const EdgeFunc<double>& weights,
                              int maxNumSets)
{
  std::vector<UnitaryAFCSet> sets;
  int numSets = 0;

  for(const TimedEdge& incoming : graph.getEdges())
  {
    if(!cmp::pos(weights(incoming)))
    {
      continue;
    }

    if(graph.underlyingVertex(incoming.getTarget()) == originalSource ||
       graph.underlyingVertex(incoming.getSource()) == originalSource)
    {
      continue;
    }

    VertexSet vertices(graph);

    if(findCut(incoming, weights, vertices))
    {
      sets.push_back(UnitaryAFCSet(incoming, vertices));
      ++numSets;

      if(maxNumSets != -1 && numSets >= maxNumSets)
      {
        break;
      }

    }
  }

  return sets;
}

VertexSet UnitaryAFCSeparator::findVertices(const TimedEdge& incoming) const
{
  std::vector<Vertex> forbiddenVertices{originalSource,
      graph.underlyingVertex(incoming.getSource()),
      graph.underlyingVertex(incoming.getTarget())};

  auto filter = [&](const TimedEdge& edge) -> bool
    {
      return !contains(forbiddenVertices, graph.underlyingVertex(edge.getTarget()));
    };

  VertexSet vertices(graph);

  std::queue<TimedVertex> nextVertices;

  const TimedVertex& source = incoming.getTarget();

  vertices.insert(source);
  nextVertices.push(source);

  while(!nextVertices.empty())
  {
    TimedVertex current = nextVertices.front();
    nextVertices.pop();

    assert(vertices.contains(current));

    for(const TimedEdge& outgoing : graph.getOutgoing(current))
    {
      if(!filter(outgoing))
      {
        continue;
      }

      TimedVertex next = outgoing.getTarget();

      if(vertices.contains(next))
      {
        continue;
      }

      vertices.insert(next);
      nextVertices.push(next);
    }
  }

  return vertices;
}

bool UnitaryAFCSeparator::findCut(const TimedEdge& incoming,
                                  const EdgeFunc<double>& weights,
                                  VertexSet& cut) const
{
  VertexSet vertices = findVertices(incoming);

  assert(vertices.contains(incoming.getTarget()));

  Graph subGraph;
  std::unordered_map<TimedVertex, Vertex> subVertices;

  for(const TimedVertex& vertex : graph.getVertices())
  {
    if(!vertices.contains(vertex))
    {
      continue;
    }

    subVertices.insert({vertex, subGraph.addVertex()});
  }

  EdgeMap<double> subWeights(subGraph, 0);

  for(const TimedEdge& edge : graph.getEdges())
  {
    const TimedVertex& source = edge.getSource();
    const TimedVertex& target = edge.getTarget();

    if(vertices.contains(source) &&
       vertices.contains(target))
    {
      Vertex subSource = subVertices[source];
      Vertex subTarget = subVertices[target];

      Edge subEdge = subGraph.addEdge(subSource, subTarget);
      subWeights.extend(subEdge, weights(edge));
    }
  }

  Vertex subSource = subVertices[incoming.getTarget()];

  MaxFlowResult result(subGraph);
  result.value = inf;

  for(const Vertex& vertex : subGraph.getVertices())
  {
    if(vertex == subSource)
    {
      continue;
    }

    if(!subGraph.getOutgoing(vertex).empty())
    {
      continue;
    }

    MaxFlowResult currentResult = computeMaxFlow(subGraph,
                                                 subSource,
                                                 vertex,
                                                 subWeights.getValues());

    assert(currentResult.cut.contains(subSource));

    if(currentResult.value < result.value)
    {
      result = currentResult;
    }
  }

  double upperBound = weights(incoming);

  if(result.value < upperBound)
  {
    for(const TimedVertex& vertex : graph.getVertices())
    {
      if(!vertices.contains(vertex))
      {
        continue;
      }

      if(result.cut.contains(subVertices.at(vertex)))
      {
        cut.insert(vertex);
      }
    }

    assert(cut.contains(incoming.getTarget()));

    return true;
  }
  else
  {
    return false;
  }
}
