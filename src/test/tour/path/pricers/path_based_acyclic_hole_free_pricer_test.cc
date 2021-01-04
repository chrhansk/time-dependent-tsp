#include "path_based_pricer_test.hh"

#include "tour/path/pricers/path_based_router_pricer.hh"

class PathBasedAcyclicHoleFreePricerTest : public PathBasedPricerTest
{
protected:
  virtual PathBasedPricer* getPricer(PathBasedProgram& program,
                                     const Instance& instance) override;

};

PathBasedPricer* PathBasedAcyclicHoleFreePricerTest::getPricer(PathBasedProgram& program,
                                                       const Instance& instance)
{
  return new PathBasedAcyclicHoleFreePricer<3>(program);
}

TEST_F(PathBasedAcyclicHoleFreePricerTest, testPathBasedAcyclicHoleFreePricer)
{
  testPricer();
}
