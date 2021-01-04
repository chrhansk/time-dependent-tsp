#ifndef ACYCLIC_RELAXATION_TEST_HH
#define ACYCLIC_RELAXATION_TEST_HH

#include <gtest/gtest.h>

#include "tour/tour.hh"

#include "instance.hh"

class AcyclicRelaxationTest : public testing::Test
{
protected:
  std::vector<InstanceInfo> infos;
public:
  AcyclicRelaxationTest();

  virtual double solveRelaxation(Instance& instance,
                                 const Tour& initialTour) = 0;

  void test(idx size);
};

#endif /* ACYCLIC_RELAXATION_TEST_HH */
