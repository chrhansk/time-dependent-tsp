#include "path_based_pricer_test.hh"

#include "tour/path/pricers/path_based_stabilizing_pricer.hh"

class PathBasedSimpleStabilizingPricerTest : public PathBasedPricerTest
{
protected:
  virtual PathBasedPricer* getPricer(PathBasedProgram& program,
                                     const Instance& instance) override;

};

PathBasedPricer* PathBasedSimpleStabilizingPricerTest::getPricer(PathBasedProgram& program,
                                                                 const Instance& instance)
{
  
  return new PathBasedSimpleStabilizingPricer(program);
}

TEST_F(PathBasedSimpleStabilizingPricerTest, testPathBasedStabilizingPricer)
{
  testPricer();
}
