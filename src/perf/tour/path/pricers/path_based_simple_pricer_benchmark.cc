#include "path_based_pricer_benchmark.hh"

#include "tour/path/pricers/path_based_router_pricer.hh"

class PathBasedSimplePricerBenchmark : public PathBasedPricerBenchmark
{
  PathBasedPricer* getPricer(PathBasedProgram& program) const override
  {
    return new PathBasedSimplePricer(program);

  };
};

int main(int argc, char *argv[])
{
  PathBasedSimplePricerBenchmark().run(argc, argv);

  return 0;
}
