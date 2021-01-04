#include "separator_test.hh"

#include "tour/timed/separators/simple_odd_cat_separator.hh"

class SimpleOddCATSeparatorTest : public SeparatorTest
{
protected:
  virtual scip::ObjSepa* getSeparator(SimpleProgram& program,
                                      const Instance& instance) override;
};

scip::ObjSepa* SimpleOddCATSeparatorTest::getSeparator(SimpleProgram& program,
                                                       const Instance& instance)
{
  return new SimpleOddCATSeparator(program);
}

TEST_F(SimpleOddCATSeparatorTest, testSubtourSeparator)
{
  testSeparator();
}
