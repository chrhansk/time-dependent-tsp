#include "tour/program_test.hh"

#include "tour/static/tour_solver.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/sparse_objective_propagator.hh"

class SparsePropagatorTest : public ProgramTest
{
  Tour solve(Instance& instance,
             const Tour& initialTour) override
  {
    SparseProgram program(initialTour,
                          instance.timedDistances,
                          initialTour.cost(instance.staticCosts));

    program.addPropagator(new SparseObjectivePropagator(program));

    auto result = program.solve();

    return *(result.tour);
  }
};

TEST_F(SparsePropagatorTest, testPropagator)
{
  test();
}
