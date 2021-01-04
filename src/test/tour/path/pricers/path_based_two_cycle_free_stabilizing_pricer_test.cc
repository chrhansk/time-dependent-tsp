#include "path_based_pricer_test.hh"

#include "tour/path/pricers/path_based_stabilizing_pricer.hh"

class PathBasedTwoCycleFreeStabilizingPricerTest : public PathBasedPricerTest
{
protected:
  virtual PathBasedPricer* getPricer(PathBasedProgram& program,
                                     const Instance& instance) override;

};

PathBasedPricer* PathBasedTwoCycleFreeStabilizingPricerTest::getPricer(PathBasedProgram& program,
                                                                       const Instance& instance)
{
  return new PathBasedTwoCycleFreeStabilizingPricer(program);
}

TEST_F(PathBasedTwoCycleFreeStabilizingPricerTest, testPathBasedStabilizingPricer)
{
  testPricer();
}
