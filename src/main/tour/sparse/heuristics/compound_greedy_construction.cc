#include "compound_greedy_construction.hh"

double CompoundGreedyConstruction::score(const TimedEdge& edge) const
{
  return combinedValues(graph.underlyingEdge(edge)) + 1e-5 * (1/ ((double) edge.travelTime()));
}

bool CompoundGreedyConstruction::startRound()
{
  SparseSolutionValues solutionValues = program.solutionValues();
  combinedValues = graph.combinedValues(solutionValues);

  return true;
}

CompoundGreedyConstruction::CompoundGreedyConstruction(SparseProgram& program,
                                                       int numRuns)
  : GreedyConstruction(program,
                       "compound_greedy",
                       "constructs tours greedily based on compound flow",
                       'c',
                       numRuns),
    combinedValues(program.getGraph().underlyingGraph(), 0.)
{

}
