#ifndef SPARSE_PRICER_TEST_HH
#define SPARSE_PRICER_TEST_HH

#include "tour/program_test.hh"

#include "tour/static/tour_solver.hh"

#include "tour/sparse/sparse_program.hh"

class SparsePricer;

class SparsePricerTest : public ProgramTest
{
protected:
  virtual SparsePricer* getPricer(SparseProgram& program,
                                  const Instance& instance) = 0;

public:
  void testPricer();

  Tour solve(Instance& instance,
             const Tour& initialTour) override;
};


#endif /* SPARSE_PRICER_TEST_HH */
