#ifndef RELAXATION_TEST_HH
#define RELAXATION_TEST_HH

#include <gtest/gtest.h>

#include "tour/tour.hh"

#include "instance.hh"

class RelaxationTest : public testing::Test
{
protected:
  std::vector<InstanceInfo> infos;
public:
  RelaxationTest();

  virtual double solveRelaxation(Instance& instance,
                                 const Tour& initialTour) = 0;

  void test();
};

#endif /* RELAXATION_TEST_HH */
