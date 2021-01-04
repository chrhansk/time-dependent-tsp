#include "tour/acyclic_relaxation_test.hh"

#include "tour/path/path_based_program.hh"
#include "tour/path/pricers/path_based_stabilizing_pricer.hh"

class PathBasedAcyclicStabilizingRelaxationTest : public AcyclicRelaxationTest
{
protected:
  PathBasedPricer* getPricer(PathBasedProgram& program,
                             const Instance& instance);

  virtual double solveRelaxation(Instance& instance,
                                 const Tour& initialTour) override;

};

double PathBasedAcyclicStabilizingRelaxationTest::solveRelaxation(Instance& instance,
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

PathBasedPricer* PathBasedAcyclicStabilizingRelaxationTest::getPricer(PathBasedProgram& program,
                                                                      const Instance& instance)
{
  return new PathBasedAcyclicHoleFreeStabilizingPricer<3>(program);
}

TEST_F(PathBasedAcyclicStabilizingRelaxationTest, testRelaxation)
{
  test(3);
}
