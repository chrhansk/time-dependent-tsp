#ifndef SPARSE_HEURISTIC_BENCHMARK_HH
#define SPARSE_HEURISTIC_BENCHMARK_HH

#include "tour/sparse/sparse_program_benchmark.hh"

#include "tour/sparse/heuristics/greedy_construction.hh"
#include "tour/sparse/sparse_program.hh"

class SparseHeuristicBenchmark : public SparseProgramBenchmark
{
public:
  SolutionResult execute(Instance& instance,
                         const Tour& initialTour,
                         int timeLimit) override;

  virtual GreedyConstruction* getHeuristic(SparseProgram& program) const = 0;
};


#endif /* SPARSE_HEURISTIC_BENCHMARK_HH */
