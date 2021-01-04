#include "sparse_separator_benchmark.hh"


SolutionResult
SparseSeparatorBenchmark::execute(Instance& instance,
                                  const Tour& initialTour,
                                  int timeLimit)
{
  auto settings = Program::Settings()
    .collectStats()
    .withSetFile(setFilePath("sparse_separator"));

  SparseProgram program(initialTour,
                        instance.timedDistances,
                        0,
                        true,
                        settings);

  addSeparator(program, instance);

  return program.solve(timeLimit);
}
