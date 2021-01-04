#include "sparse_pricer_benchmark.hh"

#include "tour/sparse/pricers/sparse_path_router_pricer.hh"

class SparseEdgePricerBenchmark : public SparsePricerBenchmark
{
  SparsePricer* getPricer(SparseProgram& program) const override
  {
    return new SparseSimplePathPricer(program);
  }
};

int main(int argc, char *argv[])
{
  SparseEdgePricerBenchmark().run(argc, argv);

  return 0;
}
