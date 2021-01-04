#include "solution_result.hh"

SolutionResult::SolutionResult(const Tour& tour,
                               const SolutionStats& solutionStats)
  : tour(tour),
    stats(solutionStats)
{
}

SolutionResult::SolutionResult(double value,
                               const SolutionStats& solutionStats)
  : relaxationValue(value),
    stats(solutionStats)
{

}
