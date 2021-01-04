#include <iostream>
#include <random>

#include "basic_test.hh"

#include "timed/augmented_edge_func.hh"

class AugmentedEdgeFuncTest : public BasicTest
{
public:
  AugmentedEdgeFuncTest()
    : BasicTest()
  {}
};

TEST_F(AugmentedEdgeFuncTest, testFunc)
{
  std::mt19937 engine(17);

  const num timeSteps = 5000;
  const idx scaleFactor = 3;

  auto distribution = std::uniform_int_distribution<>(0, timeSteps);

  auto func = AugmentedEdgeFunc::generate(graph,
                                          costs,
                                          scaleFactor,
                                          10,
                                          timeSteps,
                                          [&]() -> idx {
                                            return distribution(engine);
                                          });

  for(const Edge& edge : graph.getEdges())
  {
    const num currentCost = costs(edge);

    idx lastValue = func(edge, 0);

    for(idx currentTime = 1; currentTime < timeSteps; ++currentTime)
    {
      num currentValue = func(edge, currentTime);

      ASSERT_TRUE(currentValue >= currentCost);
      ASSERT_TRUE(currentValue <= (num) (scaleFactor * currentCost));
      idx valueDiff = std::abs(((num)lastValue) - ((num)currentValue));
      ASSERT_TRUE(valueDiff <= 1);

      lastValue = currentValue;
    }
  }

}
