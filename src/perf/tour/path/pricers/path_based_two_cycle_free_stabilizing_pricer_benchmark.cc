#include "path_based_pricer_benchmark.hh"

#include "tour/path/pricers/path_based_stabilizing_pricer.hh"

class PathBasedTwoCycleFreeStabilizingPricerBenchmark : public PathBasedPricerBenchmark
{
  PathBasedPricer* getPricer(PathBasedProgram& program) const override
  {
    return new PathBasedTwoCycleFreeStabilizingPricer(program);
  };
};

int main(int argc, char *argv[])
{
  PathBasedTwoCycleFreeStabilizingPricerBenchmark().run(argc, argv);

  return 0;
}
