#ifndef SIMPLE_PROGRAM_BENCHMARK_HH
#define SIMPLE_PROGRAM_BENCHMARK_HH

#include "tour/program_benchmark.hh"

class SimpleProgramBenchmark : public ProgramBenchmark
{
  virtual SolutionResult execute(Instance& instance,
                                 const Tour& initialTour,
                                 int timeLimit = -1) override;
};


#endif /* SIMPLE_PROGRAM_BENCHMARK_HH */
