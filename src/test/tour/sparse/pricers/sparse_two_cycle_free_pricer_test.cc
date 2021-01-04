#include "sparse_pricer_test.hh"

#include "tour/sparse/pricers/sparse_path_router_pricer.hh"

class SparseTwoCycleFreePricerTest : public SparsePricerTest
{
protected:
  virtual SparsePricer* getPricer(SparseProgram& program,
                                  const Instance& instance) override;

};

SparsePricer* SparseTwoCycleFreePricerTest::getPricer(SparseProgram& program,
                                                       const Instance& instance)
{
  return new SparseTwoCycleFreePricer(program);
}

TEST_F(SparseTwoCycleFreePricerTest, testSparseSimpleAcyclicPricer)
{
  testPricer();
}
