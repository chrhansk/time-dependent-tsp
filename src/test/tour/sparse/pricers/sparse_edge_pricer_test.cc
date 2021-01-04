#include "sparse_pricer_test.hh"

#include "tour/sparse/pricers/sparse_edge_pricer.hh"

class SparseEdgePricerTest : public SparsePricerTest
{
protected:
  virtual SparsePricer* getPricer(SparseProgram& program,
                                  const Instance& instance) override;

};

SparsePricer* SparseEdgePricerTest::getPricer(SparseProgram& program,
                                              const Instance& instance)
{
  return new SparseEdgePricer(program);
}

TEST_F(SparseEdgePricerTest, testSparseEdgePricer)
{
  testPricer();
}
