#include "separator_test.hh"

Tour SeparatorTest::solve(Instance& instance,
                          const Tour& initialTour)
{
  SimpleProgram program(initialTour, instance.timedDistances);

  program.addSeparator(getSeparator(program, instance));

  auto result = program.solve();

  return *(result.tour);
}

void SeparatorTest::testSeparator()
{
  test();
}
