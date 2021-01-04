#ifndef SEPARATOR_TEST_HH
#define SEPARATOR_TEST_HH

#include "tour/program_test.hh"

#include "tour/static/tour_solver.hh"

#include "tour/timed/simple_program.hh"

class SeparatorTest : public ProgramTest
{
protected:

  virtual scip::ObjSepa* getSeparator(SimpleProgram& program,
                                      const Instance& instance) = 0;

public:
  void testSeparator();

  Tour solve(Instance& instance,
             const Tour& initialTour) override;
};

#endif /* SEPARATOR_TEST_HH */
