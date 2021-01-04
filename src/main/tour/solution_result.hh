#ifndef SOLUTION_RESULT_HH
#define SOLUTION_RESULT_HH

#include <scip/scip.h>

#include "tour.hh"
#include "solution_stats.hh"

class SolutionResult
{
public:
  SolutionResult(const Tour& tour,
                 const SolutionStats& solutionStats);

  SolutionResult(double value,
                 const SolutionStats& solutionStats);

  std::optional<Tour> tour;
  std::optional<double> relaxationValue;
  SolutionStats stats;
};


#endif /* SOLUTION_RESULT_HH */
