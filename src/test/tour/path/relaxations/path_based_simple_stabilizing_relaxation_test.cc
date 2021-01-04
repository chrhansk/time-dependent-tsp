#include "tour/relaxation_test.hh"

#include "tour/path/path_based_program.hh"
#include "tour/path/pricers/path_based_stabilizing_pricer.hh"

class PathBasedSimpleStabilizingRelaxationTest : public RelaxationTest
{
protected:
  PathBasedPricer* getPricer(PathBasedProgram& program,
                             const Instance& instance);

  virtual double solveRelaxation(Instance& instance,
                                 const Tour& initialTour) override;

};

double PathBasedSimpleStabilizingRelaxationTest::solveRelaxation(Instance& instance,
                                                      const Tour& initialTour)
{
  PathBasedProgram program(initialTour,
                           instance.timedDistances,
                           0,
                           false,
                           Program::Settings().doSolveRelaxation());

  program.setPricer(getPricer(program, instance));

  return program.solveRelaxation();
}

PathBasedPricer* PathBasedSimpleStabilizingRelaxationTest::getPricer(PathBasedProgram& program,
                                                          const Instance& instance)
{
  return new PathBasedSimpleStabilizingPricer(program);
}

TEST_F(PathBasedSimpleStabilizingRelaxationTest, testRelaxation)
{
  test();
}
