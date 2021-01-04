#include "separator_test.hh"

#include "tour/timed/separators/cycle_separator.hh"

class CycleSeparatorTest : public SeparatorTest
{
protected:
  virtual scip::ObjSepa* getSeparator(SimpleProgram& program,
                                      const Instance& instance) override;
};

scip::ObjSepa* CycleSeparatorTest::getSeparator(SimpleProgram& program,
                                                const Instance& instance)
{
  return new CycleSeparator(program);
}

TEST_F(CycleSeparatorTest, testCycleSeparator)
{
  testSeparator();
}
