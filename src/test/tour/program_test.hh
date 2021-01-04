#ifndef PROGRAM_TEST_HH
#define PROGRAM_TEST_HH

#include <gtest/gtest.h>

#include "tour/tour.hh"

#include "instance.hh"

class ProgramTest : public testing::Test
{
protected:
  std::vector<InstanceInfo> infos;

public:
  ProgramTest();

  virtual Tour solve(Instance& instance,
                     const Tour& initialTour) = 0;

  void test();
};


#endif /* PROGRAM_TEST_HH */
