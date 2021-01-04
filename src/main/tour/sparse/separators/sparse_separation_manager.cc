#include "sparse_separation_manager.hh"

#define NAME "tdtsp_separator"

#include "tour/sparse/pricers/sparse_pricing_manager.hh"
#include "tour/sparse/sparse_solution_values.hh"

SparseSeparationManager::SparseSeparationManager(SparseProgram& program,
                                                 num maxRounds,
                                                 num maxCutsPerRound)
  : scip::ObjSepa(program.getSCIP(),
                  NAME,
                  "Separates TDTSP inequalities",
                  100000,
                  0,
                  0.0,
                  FALSE,
                  FALSE),
  program(program),
  maxRounds(maxRounds),
  maxCutsPerRound(maxCutsPerRound),
  currentRound(0)
{}

void SparseSeparationManager::addedEdge(const TimedEdge& timedEdge)
{
  for(std::unique_ptr<SparseCut>& cut : cuts)
  {
    cut->addedEdge(timedEdge);
  }
}

void SparseSeparationManager::addDualCosts(EdgeMap<double>& dualCosts,
                                           DualCostType costType) const
{
  for(const std::unique_ptr<SparseCut>& cut : cuts)
  {
    const double costCoefficient = cut->getDual(costType);

    if(cmp::zero(costCoefficient))
    {
      continue;
    }

    cut->addDualCosts(dualCosts, costType);
  }
}

void SparseSeparationManager::addSeparator(std::unique_ptr<SparseSeparator>&& separator)
{
  separators.push_back(std::move(separator));
}

SCIP_DECL_SEPAEXECLP(SparseSeparationManager::scip_execlp)
{
  *result = SCIP_DIDNOTRUN;

  int numCuts = 0;

  if(maxRounds != -1)
  {
    if(currentRound++ >= maxRounds)
    {
      *result = SCIP_DIDNOTRUN;
      return SCIP_OKAY;
    }
    else
    {
      Log(info) << "Starting separation round " << currentRound << "/" << maxRounds;
    }
  }

  SparseSolutionValues solutionValues(scip, program.getPricingManager().getVariables().getValues());

  if(maxCutsPerRound == -1)
  {
    for(std::unique_ptr<SparseSeparator>& separator : separators)
    {
      std::vector<std::unique_ptr<SparseCut>> currentCuts = separator->separate(solutionValues,
                                                                                sepa);

      for(std::unique_ptr<SparseCut>& currentCut : currentCuts)
      {
        cuts.push_back(std::move(currentCut));
        ++numCuts;
      }
    }
  }
  else
  {
    int remainingCuts = maxCutsPerRound;

    Log(info) << "Finding up to " << maxCutsPerRound << " cuts";

    assert(remainingCuts > 0);

    for(std::unique_ptr<SparseSeparator>& separator : separators)
    {
      std::vector<std::unique_ptr<SparseCut>> currentCuts = separator->separate(solutionValues,
                                                                                sepa,
                                                                                remainingCuts);

      assert(((int) currentCuts.size()) <= remainingCuts);

      for(std::unique_ptr<SparseCut>& currentCut : currentCuts)
      {
        cuts.push_back(std::move(currentCut));
        ++numCuts;
        --remainingCuts;
      }

      if(remainingCuts == 0)
      {
        break;
      }
    }
  }

  Log(info) << "Found " << numCuts << " cuts";

  *result = (numCuts > 0) ? SCIP_SEPARATED : SCIP_DIDNOTFIND;

  return SCIP_OKAY;
}

SCIP_DECL_SEPAEXITSOL(SparseSeparationManager::scip_exitsol)
{
  cuts.clear();
  separators.clear();

  return SCIP_OKAY;
}
