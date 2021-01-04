#include "sparse_relaxation_benchmark.hh"

#include "tour/sparse/pricers/sparse_path_router_pricer.hh"

typedef SparseRelaxationBenchmark<SparseAcyclicHoleFreePricer<5>> Benchmark;

int main(int argc, char *argv[])
{
  Benchmark().run(argc, argv);

  return 0;
}
