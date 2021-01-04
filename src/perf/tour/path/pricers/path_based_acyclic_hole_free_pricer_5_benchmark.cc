#include "path_based_pricer_benchmark.hh"

#include "tour/path/pricers/path_based_router_pricer.hh"

class PathBasedAcyclicHoleFreePricerBenchmark : public PathBasedPricerBenchmark
{
  PathBasedPricer* getPricer(PathBasedProgram& program) const override
  {
    return new PathBasedAcyclicHoleFreePricer<5>(program);

  };
};

int main(int argc, char *argv[])
{
  PathBasedAcyclicHoleFreePricerBenchmark().run(argc, argv);

  return 0;
}
