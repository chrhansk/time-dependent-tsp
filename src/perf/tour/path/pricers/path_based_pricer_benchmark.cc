#include "path_based_pricer_benchmark.hh"

SolutionResult
PathBasedPricerBenchmark::execute(Instance& instance,
                                  const Tour& initialTour,
                                  int timeLimit)
{
  auto settings = Program::Settings()
    .collectStats()
    .withSetFile(setFilePath("sparse_pricer"));

  PathBasedProgram program(initialTour,
                        instance.timedDistances,
                        0,
                        false,
                        settings);

  program.setPricer(getPricer(program));

  return program.solve(timeLimit);
}
