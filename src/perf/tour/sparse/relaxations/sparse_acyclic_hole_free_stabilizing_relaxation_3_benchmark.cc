#include "sparse_relaxation_benchmark.hh"

#include "tour/sparse/pricers/sparse_stabilizing_pricer.hh"
#include "tour/sparse/pricers/sparse_path_router_pricer.hh"

#include "sparse_stabilizing_relaxation_benchmark.hh"

typedef SparseStabilizingRelaxationBenchmark<SparseAcyclicHoleFreePricer<3>> Benchmark;

int main(int argc, char *argv[])
{
  Benchmark().run(argc, argv);

  return 0;
}
