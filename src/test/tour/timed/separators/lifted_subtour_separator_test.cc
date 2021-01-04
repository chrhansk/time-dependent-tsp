#include "separator_test.hh"

#include "tour/timed/separators/lifted_subtour_separator.hh"

class LiftedSubtourSeparatorTest : public SeparatorTest
{
protected:
  virtual scip::ObjSepa* getSeparator(SimpleProgram& program,
                                      const Instance& instance) override;
};

scip::ObjSepa* LiftedSubtourSeparatorTest::getSeparator(SimpleProgram& program,
                                                  const Instance& instance)
{
  const idx timeHorizon = program.getInitialTour().cost(program.getDistances());

  return new LiftedSubtourSeparator(program,
                                    instance.staticCosts,
                                    timeHorizon);
}

TEST_F(LiftedSubtourSeparatorTest, testLiftedSubtourSeparator)
{
  testSeparator();
}
