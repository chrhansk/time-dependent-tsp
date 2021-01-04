#ifndef SPARSE_SEPARATOR_TEST_HH
#define SPARSE_SEPARATOR_TEST_HH

#include "tour/program_test.hh"

#include "tour/sparse/sparse_program.hh"

class SparseSeparatorTest : public ProgramTest
{
protected:

  virtual void addSeparator(SparseProgram& program,
                            const Instance& instance) = 0;

public:
  void testSeparator();

  Tour solve(Instance& instance,
             const Tour& initialTour) override;
};

#endif /* SPARSE_SEPARATOR_TEST_HH */
