#include "relaxation_test.hh"

#include "tour/static/tour_solver.hh"

RelaxationTest::RelaxationTest()
{
  logInit();

  infos = InstanceInfo::smallInstances();
}

void RelaxationTest::test()
{
  for(InstanceInfo& info : infos)
  {
    Instance instance(info);

    if(!instance.info.relaxedObjective)
    {
      continue;
    }

    TourSolver simpleSolver(instance.graph, instance.staticCosts);

    const Tour initialTour = simpleSolver.findTour();

    const double relaxedObjective = solveRelaxation(instance, initialTour);

    EXPECT_NEAR(*instance.info.relaxedObjective, relaxedObjective, 1e-2);
  }
}
