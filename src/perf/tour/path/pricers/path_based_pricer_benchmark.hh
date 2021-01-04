#include "tour/path/path_based_program_benchmark.hh"

#include "tour/path/path_based_program.hh"

class PathBasedPricerBenchmark : public PathBasedProgramBenchmark
{
public:
  SolutionResult execute(Instance& instance,
                         const Tour& initialTour,
                         int timeLimit) override;

  virtual PathBasedPricer* getPricer(PathBasedProgram& program) const = 0;
};
