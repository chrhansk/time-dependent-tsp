#include "tour/program_test.hh"

#include "tour/relaxation_test.hh"

#include "tour/static/tour_solver.hh"

#include "tour/timed/simple_program.hh"

class SimpleProgramTest : public ProgramTest
{
public:
  Tour solve(Instance& instance,
             const Tour& initialTour) override
  {
    SimpleProgram program(initialTour, instance.timedDistances);

    auto result = program.solve();

    return *(result.tour);
  }
};

class SimpleRelaxationTest : public RelaxationTest
{
public:
  double solveRelaxation(Instance& instance,
                         const Tour& initialTour) override
  {
    SimpleProgram program(initialTour,
                          instance.timedDistances,
                          Program::Settings().doSolveRelaxation());

    return *(program.solveRelaxation().relaxationValue);
  }
};

TEST_F(SimpleProgramTest, testProgram)
{
  test();
}

TEST_F(SimpleRelaxationTest, testRelaxation)
{
  test();
}
