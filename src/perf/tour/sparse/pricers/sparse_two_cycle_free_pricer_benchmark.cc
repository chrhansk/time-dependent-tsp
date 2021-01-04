#include "sparse_pricer_benchmark.hh"

#include "tour/sparse/pricers/sparse_path_router_pricer.hh"

class SparseTwoCycleFreePricerBenchmark : public SparsePricerBenchmark
{
  SparsePricer* getPricer(SparseProgram& program) const override
  {
    return new SparseTwoCycleFreePricer(program);
  }
};

int main(int argc, char *argv[])
{
  SparseTwoCycleFreePricerBenchmark().run(argc, argv);

  return 0;
}
