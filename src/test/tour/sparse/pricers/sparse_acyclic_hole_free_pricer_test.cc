#include "sparse_pricer_test.hh"

#include "tour/sparse/pricers/sparse_path_router_pricer.hh"

class SparseAcyclicHoleFreePricerTest : public SparsePricerTest
{
protected:
  virtual SparsePricer* getPricer(SparseProgram& program,
                                  const Instance& instance) override;

};

SparsePricer* SparseAcyclicHoleFreePricerTest::getPricer(SparseProgram& program,
                                                         const Instance& instance)
{
  return new SparseAcyclicHoleFreePricer<3>(program);
}

TEST_F(SparseAcyclicHoleFreePricerTest, testSparseAcyclicPricer)
{
  testPricer();
}
