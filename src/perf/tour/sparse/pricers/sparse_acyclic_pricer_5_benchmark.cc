#include "sparse_pricer_benchmark.hh"

#include "tour/sparse/pricers/sparse_acyclic_pricer.hh"

class SparseSimpleAcyclicPricerBenchmark : public SparsePricerBenchmark
{
  SparsePricer* getPricer(SparseProgram& program) const override
  {
    return new SparseAcyclicPricer<5>(program);
  }
};

int main(int argc, char *argv[])
{
  SparseSimpleAcyclicPricerBenchmark().run(argc, argv);

  return 0;
}
