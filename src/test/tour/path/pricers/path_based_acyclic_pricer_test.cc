#include "path_based_pricer_test.hh"

#include "tour/path/pricers/path_based_router_pricer.hh"

class PathBasedAcyclicPricerTest : public PathBasedPricerTest
{
protected:
  virtual PathBasedPricer* getPricer(PathBasedProgram& program,
                                     const Instance& instance) override;

};

PathBasedPricer* PathBasedAcyclicPricerTest::getPricer(PathBasedProgram& program,
                                                       const Instance& instance)
{
  return new PathBasedAcyclicPricer<3>(program);
}

TEST_F(PathBasedAcyclicPricerTest, testPathBasedAcyclicPricer)
{
  testPricer();
}
