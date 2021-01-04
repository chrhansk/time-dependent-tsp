#include "path_based_pricer_test.hh"

#include "tour/path/pricers/path_based_stabilizing_pricer.hh"

class PathBasedAcyclicStabilizingPricerTest : public PathBasedPricerTest
{
protected:
  virtual PathBasedPricer* getPricer(PathBasedProgram& program,
                                     const Instance& instance) override;

};

PathBasedPricer* PathBasedAcyclicStabilizingPricerTest::getPricer(PathBasedProgram& program,
                                                                  const Instance& instance)
{

  return new PathBasedAcyclicStabilizingPricer<3>(program);
}

TEST_F(PathBasedAcyclicStabilizingPricerTest, testPathBasedStabilizingPricer)
{
  testPricer();
}
