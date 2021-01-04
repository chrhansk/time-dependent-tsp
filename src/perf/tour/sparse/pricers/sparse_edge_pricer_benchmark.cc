#include "sparse_pricer_benchmark.hh"

#include "tour/sparse/pricers/sparse_edge_pricer.hh"

class SparseEdgePricerBenchmark : public SparsePricerBenchmark
{
  SparsePricer* getPricer(SparseProgram& program) const override
  {
    return new SparseEdgePricer(program);
  }
};

int main(int argc, char *argv[])
{
  SparseEdgePricerBenchmark().run(argc, argv);

  return 0;
}
