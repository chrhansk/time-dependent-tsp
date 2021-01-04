#include "solution_stats.hh"

#include <scip/scipdefplugins.h>

LPStats LPStats::empty()
{
  return LPStats{0};
}

SolutionStats::SolutionStats(SCIP* scip,
                             const LPStats& lpStats)
  : lpStats(lpStats)
{
  primalBound = SCIPgetPrimalbound(scip);
  dualBound = SCIPgetDualbound(scip);
  gap = SCIPgetGap(scip);
  estimatedTreeSize = SCIPgetTreesizeEstimation(scip);

  maxDepth = SCIPgetMaxDepth(scip);
  numIterations = SCIPgetNLPIterations(scip);
  numNodes = SCIPgetNNodes(scip);

  numPricedVariables = SCIPgetNPricevars(scip);
  numVariables = SCIPgetNVars(scip);
  numConstraints = SCIPgetNConss(scip);
  numCuts = SCIPgetNPoolCuts(scip);
  firstLPTime = SCIPgetFirstLPTime(scip);

  firstDualBoundRoot = SCIPgetFirstLPDualboundRoot(scip);
  firstLowerBoundRoot = SCIPgetFirstLPLowerboundRoot(scip);
}

SolutionStats SolutionStats::empty()
{
  return SolutionStats();
}

SolutionStats::SolutionStats()
  : lpStats(LPStats::empty()),
    primalBound(0),
    dualBound(0),
    gap(0),
    estimatedTreeSize(0),
    maxDepth(0),
    numIterations(0),
    numNodes(0),
    numPricedVariables(0),
    numVariables(0),
    numConstraints(0),
    numCuts(0),
    firstLPTime(0),
    firstDualBoundRoot(0),
    firstLowerBoundRoot(0)
{
}
