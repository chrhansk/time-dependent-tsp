#include "sparse_heuristic_benchmark.hh"

SolutionResult
SparseHeuristicBenchmark::execute(Instance& instance,
                                  const Tour& initialTour,
                                  int timeLimit)
{
  auto settings = Program::Settings()
    .collectStats()
    .withSetFile(setFilePath("sparse_heuristic"));

  SparseProgram program(initialTour,
                        instance.timedDistances,
                        0,
                        true,
                        settings);
  
  program.addHeuristic(getHeuristic(program));

  return program.solve(timeLimit);
}
