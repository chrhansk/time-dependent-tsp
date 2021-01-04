#include "tour/acyclic_relaxation_test.hh"

#include "tour/path/path_based_program.hh"
#include "tour/path/pricers/path_based_router_pricer.hh"

class PathBasedTwoCycleFreeRelaxationTest : public AcyclicRelaxationTest
{
protected:
  PathBasedPricer* getPricer(PathBasedProgram& program,
                             const Instance& instance);

  virtual double solveRelaxation(Instance& instance,
                                 const Tour& initialTour) override;

};

double PathBasedTwoCycleFreeRelaxationTest::solveRelaxation(Instance& instance,
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

PathBasedPricer* PathBasedTwoCycleFreeRelaxationTest::getPricer(PathBasedProgram& program,
                                                                const Instance& instance)
{
  return new PathBasedTwoCycleFreePricer(program);
}

TEST_F(PathBasedTwoCycleFreeRelaxationTest, testRelaxation)
{
  test(2);
}
