#include "tour/acyclic_relaxation_test.hh"

#include "tour/path/path_based_program.hh"
#include "tour/path/pricers/path_based_router_pricer.hh"

class PathBasedAcyclicRelaxationTest : public AcyclicRelaxationTest
{
protected:
  PathBasedPricer* getPricer(PathBasedProgram& program,
                             const Instance& instance);

  virtual double solveRelaxation(Instance& instance,
                                 const Tour& initialTour) override;

};

double PathBasedAcyclicRelaxationTest::solveRelaxation(Instance& instance,
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

PathBasedPricer* PathBasedAcyclicRelaxationTest::getPricer(PathBasedProgram& program,
                                                           const Instance& instance)
{
  return new PathBasedAcyclicPricer<3>(program);
}

TEST_F(PathBasedAcyclicRelaxationTest, testRelaxation)
{
  test(3);
}
