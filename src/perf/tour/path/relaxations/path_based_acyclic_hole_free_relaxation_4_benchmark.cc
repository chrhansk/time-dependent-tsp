#include "path_based_relaxation_benchmark.hh"

#include "tour/path/pricers/path_based_router_pricer.hh"

typedef PathBasedRelaxationBenchmark<PathBasedAcyclicHoleFreePricer<4>> Benchmark;

int main(int argc, char *argv[])
{
  Benchmark().run(argc, argv);

  return 0;
}

