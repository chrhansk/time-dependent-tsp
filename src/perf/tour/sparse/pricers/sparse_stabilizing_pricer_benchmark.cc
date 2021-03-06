#include "sparse_pricer_benchmark.hh"

#include "tour/sparse/pricers/sparse_stabilizing_pricer.hh"
#include "tour/sparse/pricers/sparse_path_router_pricer.hh"

class SparseStabilizingPricerBenchmark : public SparsePricerBenchmark
{
  SparsePricer* getPricer(SparseProgram& program) const override
  {
    return new SparseStabilizingPricer(program,
                                       std::make_unique<SparseSimplePathPricer>(program));
  }

};

int main(int argc, char *argv[])
{
  SparseStabilizingPricerBenchmark().run(argc, argv);

  return 0;
}
