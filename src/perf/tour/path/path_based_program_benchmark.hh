#ifndef PATH_BASED_PROGRAM_BENCHMARK_HH
#define PATH_BASED_PROGRAM_BENCHMARK_HH

#include "tour/program_benchmark.hh"
#include "tour/path/path_based_program.hh"

class PathBasedProgramBenchmark : public ProgramBenchmark
{
  virtual SolutionResult execute(Instance& instance,
                                 const Tour& initialTour,
                                 int timeLimit = -1) override
  {
    PathBasedProgram program(initialTour,
                             instance.timedDistances,
                             0.,
                             true,
                             Program::Settings().collectStats());

    return program.solve(timeLimit);
  }
};


#endif /* PATH_BASED_PROGRAM_BENCHMARK_HH */
