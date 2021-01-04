#include "path_based_pricer_benchmark.hh"

#include "tour/path/pricers/path_based_router_pricer.hh"

class PathBasedTwoCycleFreePricerBenchmark : public PathBasedPricerBenchmark
{
  PathBasedPricer* getPricer(PathBasedProgram& program) const override
  {
    return new PathBasedTwoCycleFreePricer(program);

  };
};

int main(int argc, char *argv[])
{
  PathBasedTwoCycleFreePricerBenchmark().run(argc, argv);

  return 0;
}
