#include "separator_test.hh"

#include "tour/timed/separators/simple_dk_separator.hh"

class SimpleDKSeparatorTest : public SeparatorTest
{
protected:
  virtual scip::ObjSepa* getSeparator(SimpleProgram& program,
                                      const Instance& instance) override;
};

scip::ObjSepa* SimpleDKSeparatorTest::getSeparator(SimpleProgram& program,
                                                   const Instance& instance)
{
  return new SimpleDKSeparator(program);
}

TEST_F(SimpleDKSeparatorTest, testSubtourSeparator)
{
  testSeparator();
}
