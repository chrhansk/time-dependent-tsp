#include "sparse_pricer_benchmark.hh"

#include "tour/sparse/pricers/sparse_path_router_pricer.hh"

class SparseSimpleAcyclicPricerBenchmark : public SparsePricerBenchmark
{
  SparsePricer* getPricer(SparseProgram& program) const override
  {
    return new SparseAcyclicHoleFreePricer<5>(program);
  }
};

int main(int argc, char *argv[])
{
  SparseSimpleAcyclicPricerBenchmark().run(argc, argv);

  return 0;
}
