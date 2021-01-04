#include "path_based_pricer_test.hh"

#include "tour/sparse/pricers/sparse_pricer.hh"

Tour PathBasedPricerTest::solve(Instance& instance,
                                const Tour& initialTour)
{
  PathBasedProgram program(initialTour,
                           instance.timedDistances,
                           0,
                           false);

  program.setPricer(getPricer(program, instance));

  auto result = program.solve();

  return *(result.tour);
}
/*
double PathBasedPricerTest::solveRelaxation(Instance& instance,
                                            const Tour& initialTour)
{
  PathBasedProgram program(initialTour,
                           instance.timedDistances,
                           0,
                           false,
                           Program::Settings().doSolveRelaxation());

  program.setPricer(getPricer(program, instance));

  return program.solveRelaxation();
}*/

void PathBasedPricerTest::testPricer()
{
  test();
}
