#ifndef SPARSE_SEPARATOR_BENCHMARK_HH
#define SPARSE_SEPARATOR_BENCHMARK_HH

#include "tour/program_benchmark.hh"

#include "tour/sparse/sparse_program.hh"

class SparseSeparatorBenchmark : public ProgramBenchmark
{
protected:
  virtual void addSeparator(SparseProgram& program,
                            const Instance& instance) = 0;

  virtual SolutionResult execute(Instance& instance,
                                 const Tour& initialTour,
                                 int timeLimit = -1) override;
};


#endif /* SPARSE_SEPARATOR_BENCHMARK_HH */
