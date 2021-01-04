#include "value_greedy_construction.hh"

double ValueGreedyConstruction::score(const TimedEdge& edge) const
{
  SparseSolutionValues solutionValues = program.solutionValues();
  return solutionValues(edge) + 1e-5 * (1/ ((double) edge.travelTime()));
}

ValueGreedyConstruction::ValueGreedyConstruction(SparseProgram& program,
                                                 int numRuns)
  : GreedyConstruction(program,
                       "value_greedy",
                       "constructs tours greedily based on flow values",
                       'c',
                       numRuns)
{

}
