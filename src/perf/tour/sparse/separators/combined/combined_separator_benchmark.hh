#ifndef COMBINED_SEPARATOR_BENCHMARK_HH
#define COMBINED_SEPARATOR_BENCHMARK_HH

#include "tour/program_benchmark.hh"

#include "tour/sparse/sparse_program.hh"

class CombinedSeparatorBenchmark : public ProgramBenchmark
{
protected:
  virtual void addSeparators(SparseProgram& program,
                             const Instance& instance) = 0;

  virtual SolutionResult execute(Instance& instance,
                                 const Tour& initialTour,
                                 int timeLimit = -1) override;
};


#endif /* COMBINED_SEPARATOR_BENCHMARK_HH */
