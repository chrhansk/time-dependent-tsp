#ifndef SPARSE_PROGRAM_BENCHMARK_HH
#define SPARSE_PROGRAM_BENCHMARK_HH

#include "tour/program_benchmark.hh"
#include "tour/sparse/sparse_program.hh"

class SparseProgramBenchmark : public ProgramBenchmark
{
protected:
  virtual SolutionResult execute(Instance& instance,
                                 const Tour& initialTour,
                                 int timeLimit = -1) override
  {
    SparseProgram program(initialTour,
                          instance.timedDistances,
                          0.,
                          true,
                          Program::Settings().collectStats());

    return program.solve(timeLimit);
  }
};




#endif /* SPARSE_PROGRAM_BENCHMARK_HH */
