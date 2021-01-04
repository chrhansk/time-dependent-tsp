#include "path_based_pricer_benchmark.hh"

#include "tour/path/pricers/path_based_stabilizing_pricer.hh"

class PathBasedSimpleStabilizingPricerBenchmark : public PathBasedPricerBenchmark
{
  PathBasedPricer* getPricer(PathBasedProgram& program) const override
  {
    return new PathBasedSimpleStabilizingPricer(program);

  };
};

int main(int argc, char *argv[])
{
  PathBasedSimpleStabilizingPricerBenchmark().run(argc, argv);

  return 0;
}
