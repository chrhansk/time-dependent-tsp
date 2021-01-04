#include "sparse_edge_pricer.hh"

SparseEdgePricer::SparseEdgePricer(SparseProgram& program,
                                   idx numEdges)
  : SparsePricer(program),
    originalGraph(graph.underlyingGraph()),
    //dualCosts(originalGraph, 0),
    numEdges(numEdges)
{
}

EdgeMap<std::vector<SparseEdgePricer::Candidate>>
SparseEdgePricer::findCandidates(const EdgeFunc<double>& reducedCosts,
                                 std::optional<double>& minReducedCost)
{
  //createDualCosts();

  EdgeSet forbiddenEdges = program.getForbiddenEdges();

  uint numCandidates = 0;

  const bool solveRelaxation = program.getSettings().solveRelaxation;

  EdgeMap<std::vector<Candidate>> candidates(originalGraph,
                                             std::vector<Candidate>{});

  for(const Edge& edge : originalGraph.getEdges())
  {
    if(forbiddenEdges.contains(edge))
    {
      continue;
    }

    for(const TimedEdge& timedEdge : graph.getTimedEdges(edge))
    {
      const double edgeCost = reducedCosts(timedEdge);

      if(cmp::gt(timedEdge.getTarget().getTime(), program.upperBound()) &&
         !solveRelaxation)
      {
        continue;
      }

      if(graph.underlyingVertex(timedEdge.getTarget()) == program.getSource())
      {
        if(cmp::lt(timedEdge.getTarget().getTime(), program.lowerBound()) &&
           !solveRelaxation)
        {
          continue;
        }
      }

      if(minReducedCost)
      {
        minReducedCost = std::min(*minReducedCost, edgeCost);
      }
      else
      {
        minReducedCost = edgeCost;
      }

      if(cmp::neg(edgeCost))
      {
        ++numCandidates;
        candidates(edge).push_back(Candidate{timedEdge, edgeCost});
      }
    }
  }

  if(numCandidates != 0)
  {
    Log(info) << "Found " << numCandidates << " edges";
  }

  return candidates;
}

SparsePricingResult
SparseEdgePricer::performPricing(DualCostType costType)
{
  EdgeMap<double> dualValues = program.getDualValues(costType);

  EdgeMap<std::vector<Candidate>> candidates;

  std::optional<double> minReducedCost;

  if(costType == DualCostType::SIMPLE)
  {
    candidates = findCandidates(ReducedCosts(dualValues.getValues(), graph.travelTimes()),
                                minReducedCost);
  }
  else
  {
    candidates = findCandidates(FarkasCosts(dualValues.getValues()),
                                minReducedCost);
  }

  for(const Edge& edge : originalGraph.getEdges())
  {
    std::sort(std::begin(candidates(edge)), std::end(candidates(edge)),
              [](const Candidate& first, const Candidate& second) -> bool
              {
                return first.cost < second.cost;
              });
  }

  idx numCandidates = 0;

  std::vector<TimedEdge> edges;

  for(const Edge& edge : originalGraph.getEdges())
  {
    std::vector<Candidate>& currentCandidates = candidates(edge);

    currentCandidates.resize(std::min((idx) currentCandidates.size(), numEdges));

    for(const Candidate& candidate : currentCandidates)
    {
      edges.push_back(candidate.timedEdge);

      ++numCandidates;
    }
  }

  const idx numOriginalVertices = graph.underlyingGraph().getVertices().size();

  std::optional<double> lowerBound;
  
  if(minReducedCost)
  {
    minReducedCost = std::min(*minReducedCost, 0.);

    lowerBound = SCIPgetLPObjval(scip) + (*minReducedCost) * numOriginalVertices;
  }
  
  Log(info) << "Adding " << edges.size() << " edges";

  return SparsePricingResult(edges, lowerBound);

}
