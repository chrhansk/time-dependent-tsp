#include "program_test.hh"

#include "tour/static/tour_solver.hh"

ProgramTest::ProgramTest()
{
  logInit();

  infos = InstanceInfo::smallInstances();
}

void ProgramTest::test()
{
  for(const InstanceInfo& info : infos)
  {
    Instance instance(info);

    TourSolver simpleSolver(instance.graph, instance.staticCosts);

    Tour staticTour = simpleSolver.findTour();

    Tour timedTour = solve(instance, staticTour);

    TimedDistanceEvaluator timedEvaluator(instance.timedDistances);

    ASSERT_EQ(instance.info.optimalObjective, timedEvaluator(timedTour));
  }
}
