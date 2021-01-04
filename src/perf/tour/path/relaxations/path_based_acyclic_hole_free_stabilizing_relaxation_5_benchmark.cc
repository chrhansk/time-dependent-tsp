#include "path_based_relaxation_benchmark.hh"

#include "tour/path/pricers/path_based_stabilizing_pricer.hh"

typedef PathBasedRelaxationBenchmark<PathBasedAcyclicHoleFreeStabilizingPricer<5>> Benchmark;

int main(int argc, char *argv[])
{
  Benchmark().run(argc, argv);

  return 0;
}

