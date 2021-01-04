#include "simple_program_benchmark.hh"

#include "tour/timed/simple_program.hh"

SolutionResult SimpleProgramBenchmark::execute(Instance& instance,
                                               const Tour& initialTour,
                                               int timeLimit)
{
  SimpleProgram program(initialTour,
                        instance.timedDistances,
                        Program::Settings().collectStats());
  return program.solve(timeLimit);
}


int main(int argc, char *argv[])
{
  SimpleProgramBenchmark().run(argc, argv);

  return 0;
}
