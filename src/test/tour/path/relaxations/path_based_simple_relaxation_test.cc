#include "tour/relaxation_test.hh"

#include "tour/path/path_based_program.hh"
#include "tour/path/pricers/path_based_router_pricer.hh"

class PathBasedSimpleRelaxationTest : public RelaxationTest
{
protected:
  PathBasedPricer* getPricer(PathBasedProgram& program,
                             const Instance& instance);

  virtual double solveRelaxation(Instance& instance,
                                 const Tour& initialTour) override;

};

double PathBasedSimpleRelaxationTest::solveRelaxation(Instance& instance,
                                                      const Tour& initialTour)
{
  PathBasedProgram program(initialTour,
                           instance.timedDistances,
                           0,
                           false,
                           Program::Settings().doSolveRelaxation());

  program.setPricer(getPricer(program, instance));

  return *(program.solveRelaxation().relaxationValue);
}

PathBasedPricer* PathBasedSimpleRelaxationTest::getPricer(PathBasedProgram& program,
                                                          const Instance& instance)
{
  return new PathBasedSimplePricer(program);
}

TEST_F(PathBasedSimpleRelaxationTest, testRelaxation)
{
  test();
}
