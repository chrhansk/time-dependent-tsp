#ifndef SOLUTION_STATS_HH
#define SOLUTION_STATS_HH

#include <scip/scip.h>

#include "util.hh"

struct LPStats
{
  idx numLPs;
  double avgRows;
  double avgCols;

  idx maxRows;
  idx maxCols;

  idx minRows;
  idx minCols;

  double LProotObjVal;
  bool rootLPSolved;
  double rootSolvingTime;

  static LPStats empty();
};

struct SolutionStats
{
private:
  SolutionStats();
public:
  SolutionStats(SCIP* scip, const LPStats& lpStats);
  LPStats lpStats;

  double primalBound;
  double dualBound;
  double gap;
  double estimatedTreeSize;

  idx maxDepth;
  idx numIterations;
  idx numNodes;

  idx numPricedVariables;
  idx numVariables;
  idx numConstraints;

  idx numCuts;
  double firstLPTime;

  double firstDualBoundRoot;
  double firstLowerBoundRoot;

  static SolutionStats empty();

};

#endif /* SOLUTION_STATS_HH */
