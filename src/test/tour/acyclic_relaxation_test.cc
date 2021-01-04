#include "acyclic_relaxation_test.hh"

#include "tour/static/tour_solver.hh"

AcyclicRelaxationTest::AcyclicRelaxationTest()
{
  logInit();

  infos = InstanceInfo::smallInstances();
}


void AcyclicRelaxationTest::test(idx size)
{
  for(InstanceInfo& info : infos)
  {
    Instance instance(info);

    std::optional<double> expectedObjective = instance.info.relaxedAcyclicObjective(size);

    if(!expectedObjective)
    {
      continue;
    }

    TourSolver simpleSolver(instance.graph, instance.staticCosts);

    const Tour initialTour = simpleSolver.findTour();

    const double relaxedObjective = solveRelaxation(instance, initialTour);

    EXPECT_NEAR(*expectedObjective, relaxedObjective, 1e-2);
  }
}

