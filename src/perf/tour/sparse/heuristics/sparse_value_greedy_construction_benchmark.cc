#include "sparse_heuristic_benchmark.hh"

#include "tour/sparse/heuristics/value_greedy_construction.hh"

class SparseValueGreedyConstructionBenchmark : public SparseHeuristicBenchmark
{
  GreedyConstruction* getHeuristic(SparseProgram& program) const override
  {
    return new ValueGreedyConstruction(program);
  }
};

int main(int argc, char *argv[])
{
  SparseValueGreedyConstructionBenchmark().run(argc, argv);

  return 0;
}
