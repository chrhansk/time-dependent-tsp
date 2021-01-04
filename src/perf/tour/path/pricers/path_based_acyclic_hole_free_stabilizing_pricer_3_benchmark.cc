#include "path_based_pricer_benchmark.hh"

#include "tour/path/pricers/path_based_stabilizing_pricer.hh"

class PathBasedAcyclicHoleFreeStabilizingPricerBenchmark : public PathBasedPricerBenchmark
{
  PathBasedPricer* getPricer(PathBasedProgram& program) const override
  {
    return new PathBasedAcyclicHoleFreeStabilizingPricer<3>(program);

  };
};

int main(int argc, char *argv[])
{
  PathBasedAcyclicHoleFreeStabilizingPricerBenchmark().run(argc, argv);

  return 0;
}
