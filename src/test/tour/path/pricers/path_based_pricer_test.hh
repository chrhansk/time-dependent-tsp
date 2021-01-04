#ifndef PATH_BASED_PRICER_TEST_HH
#define PATH_BASED_PRICER_TEST_HH

#include "tour/program_test.hh"

#include "tour/static/tour_solver.hh"

#include "tour/path/path_based_program.hh"

class PathBasedPricer;

class PathBasedPricerTest : public ProgramTest
{
protected:
  virtual PathBasedPricer* getPricer(PathBasedProgram& program,
                                     const Instance& instance) = 0;

public:
  void testPricer();

  Tour solve(Instance& instance,
             const Tour& initialTour) override;
};

#endif /* PATH_BASED_PRICER_TEST_HH */
