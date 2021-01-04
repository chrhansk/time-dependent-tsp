#include "tour/program_test.hh"

#include "tour/relaxation_test.hh"

#include "tour/static/tour_solver.hh"

#include "tour/sparse/sparse_program.hh"

class SparseProgramTest : public ProgramTest
{
public:
  Tour solve(Instance& instance,
             const Tour& initialTour) override
  {
    SparseProgram program(initialTour,
                          instance.timedDistances,
                          initialTour.cost(instance.staticCosts));

    auto result = program.solve();

    return *(result.tour);
  }
};

class SparseRelaxationTest : public RelaxationTest
{
public:
  double solveRelaxation(Instance& instance,
                         const Tour& initialTour) override
  {
    SparseProgram program(initialTour,
                          instance.timedDistances,
                          0,
                          true,
                          Program::Settings().doSolveRelaxation());

    return *(program.solveRelaxation().relaxationValue);
  }
};

TEST_F(SparseProgramTest, testProgram)
{
  test();
}

TEST_F(SparseRelaxationTest, testRelaxation)
{
  test();
}
