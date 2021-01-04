#include "sparse_pricer_test.hh"

#include "tour/sparse/pricers/sparse_acyclic_pricer.hh"

class SparseAcyclicPricerTest : public SparsePricerTest
{
protected:
  virtual SparsePricer* getPricer(SparseProgram& program,
                                  const Instance& instance) override;

};

SparsePricer* SparseAcyclicPricerTest::getPricer(SparseProgram& program,
                                              const Instance& instance)
{
  return new SparseAcyclicPricer<3>(program);
}

TEST_F(SparseAcyclicPricerTest, testSparseAcyclicPricer)
{
  testPricer();
}
