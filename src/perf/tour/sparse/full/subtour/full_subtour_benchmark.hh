#ifndef FULL_SUBTOUR_BENCHMARK_HH
#define FULL_SUBTOUR_BENCHMARK_HH

#include "tour/program_benchmark.hh"

#include "tour/sparse/sparse_program.hh"

class FullSubtourBenchmark : public ProgramBenchmark
{
protected:
  virtual SolutionResult execute(Instance& instance,
                                 const Tour& initialTour,
                                 int timeLimit = -1) override;

  virtual void addSeparator(SparseProgram& program,
                            SparseSeparationManager* separator,
                            const Instance& instance) = 0;
};


#endif /* FULL_SUBTOUR_BENCHMARK_HH */
