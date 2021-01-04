#include "combined_separator_benchmark.hh"


SolutionResult
CombinedSeparatorBenchmark::execute(Instance& instance,
                                    const Tour& initialTour,
                                    int timeLimit)
{
  auto settings = Program::Settings()
    .collectStats()
    .withSetFile(setFilePath("sparse_combined_separators"));

  SparseProgram program(initialTour,
                        instance.timedDistances,
                        0,
                        true,
                        settings);

  addSeparators(program, instance);

  return program.solve(timeLimit);
}
