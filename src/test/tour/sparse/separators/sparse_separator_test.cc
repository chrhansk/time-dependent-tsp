#include "sparse_separator_test.hh"

#include "tour/static/tour_solver.hh"

#include "tour/sparse/sparse_program.hh"

Tour SparseSeparatorTest::solve(Instance& instance,
                                const Tour& initialTour)
{
  SparseProgram program(initialTour,
                        instance.timedDistances,
                        initialTour.cost(instance.staticCosts));

  addSeparator(program, instance);

  auto result = program.solve();

  return *(result.tour);
}

void SparseSeparatorTest::testSeparator()
{
  test();
}
