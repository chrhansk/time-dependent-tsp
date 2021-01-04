#include "tour/program_test.hh"

#include "tour/relaxation_test.hh"

#include "tour/static/tour_solver.hh"

#include "tour/path/path_based_program.hh"

class PathBasedProgramTest : public ProgramTest
{
public:
  Tour solve(Instance& instance,
             const Tour& initialTour) override
  {
    PathBasedProgram program(initialTour, instance.timedDistances);

    auto result = program.solve();

    return *(result.tour);
  }
};

class PathBasedRelaxationTest : public RelaxationTest
{
public:
  double solveRelaxation(Instance& instance,
                         const Tour& initialTour) override
  {
    PathBasedProgram program(initialTour,
                             instance.timedDistances,
                             0,
                             true,
                             Program::Settings().doSolveRelaxation());

    return *(program.solveRelaxation().relaxationValue);
  }
};

TEST_F(PathBasedProgramTest, testProgram)
{
  test();
}

TEST_F(PathBasedRelaxationTest, testRelaxation)
{
  test();
}
