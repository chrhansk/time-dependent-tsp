#include "sparse_pricer_test.hh"

#include "tour/sparse/pricers/sparse_path_router_pricer.hh"

class SparsePathPricerTest : public SparsePricerTest
{
protected:
  virtual SparsePricer* getPricer(SparseProgram& program,
                                  const Instance& instance) override;

};

SparsePricer* SparsePathPricerTest::getPricer(SparseProgram& program,
                                              const Instance& instance)
{
  return new SparseSimplePathPricer(program);
}

TEST_F(SparsePathPricerTest, testSparsePathPricer)
{
  testPricer();
}
