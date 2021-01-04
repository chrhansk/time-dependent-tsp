#include "separator_test.hh"

#include "tour/timed/separators/two_cycle_separator.hh"

class TwoCycleSeparatorTest : public SeparatorTest
{
protected:
  virtual scip::ObjSepa* getSeparator(SimpleProgram& program,
                                      const Instance& instance) override;
};

scip::ObjSepa* TwoCycleSeparatorTest::getSeparator(SimpleProgram& program,
                                                   const Instance& instance)
{
  return new TwoCycleSeparator(program);
}

TEST_F(TwoCycleSeparatorTest, testTwoCycleSeparator)
{
  testSeparator();
}
