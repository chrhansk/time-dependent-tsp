#include "separator_test.hh"

#include "tour/timed/separators/simple_odd_path_free_separator.hh"

class SimpleOddPathFreeSeparatorTest : public SeparatorTest
{
protected:
  virtual scip::ObjSepa* getSeparator(SimpleProgram& program,
                                      const Instance& instance) override;
};

scip::ObjSepa* SimpleOddPathFreeSeparatorTest::getSeparator(SimpleProgram& program,
                                                            const Instance& instance)
{
  return new SimpleOddPathFreeSeparator(program);
}

TEST_F(SimpleOddPathFreeSeparatorTest, testOddPathFree)
{
  testSeparator();
}
