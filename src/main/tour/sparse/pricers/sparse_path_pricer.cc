#include "sparse_path_pricer.hh"

#include "timed/router/simple_time_expanded_router.hh"

SparsePathPricer::SparsePathPricer(SparseProgram& program,
                                   idx numPaths)
  : SparsePricer(program),
    numPaths(numPaths)
{
}

SparsePricingResult
SparsePathPricer::performPricing(DualCostType costType)
{
  EdgeMap<double> dualValues = program.getDualValues(costType);

  std::optional<double> minReducedCost;

  std::vector<TimedPath> paths;

  const bool solveRelaxation = program.getSettings().solveRelaxation;

  std::optional<double> lowerTimeBound;
  std::optional<double> upperTimeBound;

  const EdgeSet forbiddenEdges = program.getForbiddenEdges();

  if(!solveRelaxation)
  {
    lowerTimeBound = program.lowerBound();
    upperTimeBound = program.upperBound();
  }

  if(costType == DualCostType::SIMPLE)
  {
    paths = findPaths(ReducedCosts(dualValues.getValues(), graph.travelTimes()),
                      forbiddenEdges,
                      lowerTimeBound,
                      upperTimeBound,
                      minReducedCost);
  }
  else
  {
    paths = findPaths(FarkasCosts(dualValues.getValues()),
                      forbiddenEdges,
                      lowerTimeBound,
                      upperTimeBound,
                      minReducedCost);
  }

  if(paths.empty())
  {
    Log(info) << "Could not find paths";

    return SparsePricingResult::empty();
  }
  else
  {
    Log(info) << "Found "
              << paths.size()
              << " paths";

    std::optional<double> lowerBound;

    if(minReducedCost)
    {
      assert(*minReducedCost <= 0.);
      lowerBound = SCIPgetLPObjval(scip) + *minReducedCost;
    }

    return SparsePricingResult(paths, lowerBound);
  }
}
