#include "time_greedy_construction.hh"

double TimeGreedyConstruction::score(const TimedEdge& edge) const
{
  return 1/ ((double) edge.travelTime());
}

TimeGreedyConstruction::TimeGreedyConstruction(SparseProgram& program,
                                               int numRuns)
  : GreedyConstruction(program,
                       "time_greedy",
                       "constructs tours greedily based on travel times",
                       't',
                       numRuns),
    combinedValues(program.getGraph().underlyingGraph(), 0.)
{

}
