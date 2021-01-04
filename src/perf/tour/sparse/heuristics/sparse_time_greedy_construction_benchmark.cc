#include "sparse_heuristic_benchmark.hh"

#include "tour/sparse/heuristics/time_greedy_construction.hh"

class SparseTimeGreedyConstructionBenchmark : public SparseHeuristicBenchmark
{
  GreedyConstruction* getHeuristic(SparseProgram& program) const override
  {
    return new TimeGreedyConstruction(program);
  }
};

int main(int argc, char *argv[])
{
  SparseTimeGreedyConstructionBenchmark().run(argc, argv);

  return 0;
}
