#include "odd_cat_separator.hh"

#include <sstream>

#include "log.hh"

#include "graph/edge_set.hh"
#include "router/odd_cycle.hh"

static double cutoff = 1e-5;

std::vector<std::vector<Edge>>
OddCATSeparator::separate(const EdgeFunc<double>& weights,
                          int maxNumCATs)
{
  Graph incompatGraph(0, {});
  EdgeMap<double> incompatWeights(incompatGraph, 0);
  VertexMap<Edge> originalEdges(incompatGraph, Edge());

  createIncompatibilityGraph(weights,
                             incompatGraph,
                             incompatWeights,
                             originalEdges);

  auto oddCycles = findOddCycles(incompatGraph,
                                 incompatWeights.getValues(),
                                 1.);

  std::vector<std::vector<Edge>> cycles;

  int numCATs = 0;

  for(const auto& oddCycle : oddCycles)
  {
    std::vector<Edge> edgeCycle;

    for(const Vertex& incompatVertex : oddCycle)
    {
      edgeCycle.push_back(originalEdges(incompatVertex));
    }

    assert((edgeCycle.size() % 2) == 1);
    assert(validCycle(edgeCycle));

    double violation = getViolation(weights, edgeCycle);

    if(violation < cutoff)
    {
      continue;
    }

    if(debuggingEnabled())
    {
      std::ostringstream namebuf;

      for(const Edge& edge : edgeCycle)
      {
        namebuf << edge.getSource()
                << "_"
                << edge.getTarget()
                << ", ";
      }

      Log(debug) << "Found cycle ("
                 << namebuf.str()
                 << ") with a violation of "
                 << violation;
    }

    cycles.push_back(std::move(edgeCycle));
    ++numCATs;

    if(maxNumCATs != -1 && numCATs >= maxNumCATs)
    {
      break;
    }

  }

  return cycles;
}

double OddCATSeparator::getViolation(const EdgeFunc<double>& weights,
                                     const std::vector<Edge>& cycle) const
{
  assert((cycle.size() % 2) == 1);

  double violation = 0;

  for(const Edge& edge : cycle)
  {
    violation += weights(edge);
  }

  violation -= (cycle.size() - 1 ) / 2.;

  return violation;
}

void
OddCATSeparator::createIncompatibilityGraph(const EdgeFunc<double>& weights,
                                            Graph& incompatGraph,
                                            EdgeMap<double>& incompatWeights,
                                            VertexMap<Edge>& originalEdges) const
{
  incompatGraph = Graph(0, {});

  EdgeMap<Vertex>edgeVertices(graph, Vertex());

  incompatWeights = EdgeMap<double>(incompatGraph, 0);
  originalEdges = VertexMap<Edge>(incompatGraph, Edge());

  auto nonZero= [&](const Edge& edge) -> bool
    {
      return weights(edge) >= cutoff;
    };

  for(const Edge& edge : graph.getEdges())
  {
    if(nonZero(edge))
    {
      Vertex edgeVertex = incompatGraph.addVertex();
      edgeVertices(edge) = edgeVertex;
      originalEdges.addVertex(edgeVertex, edge);
    }
  }

  for(const Edge& edge : graph.getEdges())
  {
    if(!nonZero(edge))
    {
      continue;
    }

    for(const Edge& other : graph.getAdjacentEdges(edge.getSource()))
    {
      if(!nonZero(other))
      {
        continue;
      }

      if(areIncompatible(edge, other))
      {
        Edge incompatEdge = incompatGraph.addEdge(edgeVertices(edge), edgeVertices(other));

        double incompatWeight = 1. - weights(edge) - weights(other);

        incompatWeights.extend(incompatEdge, incompatWeight);
      }
    }

    for(const Edge& other : graph.getAdjacentEdges(edge.getTarget()))
    {
      if(!nonZero(other))
      {
        continue;
      }

      if(areIncompatible(edge, other))
      {
        Edge incompatEdge = incompatGraph.addEdge(edgeVertices(edge), edgeVertices(other));

        double incompatWeight = 1. - weights(edge) - weights(other);

        incompatWeights.extend(incompatEdge, incompatWeight);
      }
    }
  }
}

bool OddCATSeparator::validCycle(const std::vector<Edge>& cycle) const
{
  if(cycle.size() <= 1)
  {
    return false;
  }

  for(idx i = 0; i < cycle.size() - 1; ++i)
  {
    if(!areIncompatible(cycle[i], cycle[i + 1]))
    {
      return false;
    }
  }

  return areIncompatible(cycle.front(), cycle.back());
}


bool OddCATSeparator::areIncompatible(const Edge& first, const Edge& second) const
{
  if(first == second)
  {
    return false;
  }

  if(first.getSource() == second.getSource())
  {
    return true;
  }

  if(first.getTarget() == second.getTarget())
  {
    return true;
  }

  if(first.getTarget() == second.getSource() &&
     second.getTarget() == first.getSource())
  {
    return true;
  }

  return false;
}
