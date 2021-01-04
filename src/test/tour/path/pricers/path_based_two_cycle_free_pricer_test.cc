#include "path_based_pricer_test.hh"

#include "tour/path/pricers/path_based_router_pricer.hh"

class PathBasedTwoCycleFreePricerTest : public PathBasedPricerTest
{
protected:
  virtual PathBasedPricer* getPricer(PathBasedProgram& program,
                                     const Instance& instance) override;

};

PathBasedPricer* PathBasedTwoCycleFreePricerTest::getPricer(PathBasedProgram& program,
                                                            const Instance& instance)
{
  return new PathBasedTwoCycleFreePricer(program);
}

TEST_F(PathBasedTwoCycleFreePricerTest, testPathBasedTwoCycleFreePricer)
{
  testPricer();
}
