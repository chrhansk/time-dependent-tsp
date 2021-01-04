#include "path_based_pricer_test.hh"

#include "tour/path/pricers/path_based_stabilizing_pricer.hh"

class PathBasedAcyclicHoleFreeStabilizingPricerTest : public PathBasedPricerTest
{
protected:
  virtual PathBasedPricer* getPricer(PathBasedProgram& program,
                                     const Instance& instance) override;

};

PathBasedPricer* PathBasedAcyclicHoleFreeStabilizingPricerTest::getPricer(PathBasedProgram& program,
                                                                          const Instance& instance)
{

  return new PathBasedAcyclicHoleFreeStabilizingPricer<3>(program);
}

TEST_F(PathBasedAcyclicHoleFreeStabilizingPricerTest, testPathBasedStabilizingPricer)
{
  testPricer();
}
