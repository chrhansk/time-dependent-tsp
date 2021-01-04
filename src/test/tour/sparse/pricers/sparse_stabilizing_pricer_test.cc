#include "sparse_pricer_test.hh"

#include "tour/sparse/pricers/sparse_path_router_pricer.hh"
#include "tour/sparse/pricers/sparse_stabilizing_pricer.hh"

class SparseStabilizingPricerTest : public SparsePricerTest
{
protected:
  virtual SparsePricer* getPricer(SparseProgram& program,
                                  const Instance& instance) override;

};

SparsePricer* SparseStabilizingPricerTest::getPricer(SparseProgram& program,
                                                     const Instance& instance)
{
  return new SparseStabilizingPricer(program,
                                     std::make_unique<SparseSimplePathPricer>(program));
}

TEST_F(SparseStabilizingPricerTest, testSparseStabilizingPricer)
{
  testPricer();
}
