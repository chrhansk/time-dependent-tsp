#include "sparse_heuristic_benchmark.hh"

#include "tour/sparse/heuristics/compound_greedy_construction.hh"

class SparseCompoundGreedyConstructionBenchmark : public SparseHeuristicBenchmark
{
  GreedyConstruction* getHeuristic(SparseProgram& program) const override
  {
    return new CompoundGreedyConstruction(program);
  }
};

int main(int argc, char *argv[])
{
  SparseCompoundGreedyConstructionBenchmark().run(argc, argv);

  return 0;
}
