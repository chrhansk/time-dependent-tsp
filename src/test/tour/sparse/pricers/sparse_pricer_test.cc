#include "sparse_pricer_test.hh"

#include "tour/sparse/pricers/sparse_pricer.hh"

Tour SparsePricerTest::solve(Instance& instance,
                             const Tour& initialTour)
{
  SparseProgram program(initialTour,
                        instance.timedDistances,
                        0,
                        false);

  program.setPricer(getPricer(program, instance));

  auto result = program.solve();

  return *(result.tour);
}

void SparsePricerTest::testPricer()
{
  test();
}
