#include "separator_test.hh"

#include "tour/timed/separators/subtour_separator.hh"

class SubtourSeparatorTest : public SeparatorTest
{
protected:
  virtual scip::ObjSepa* getSeparator(SimpleProgram& program,
                                      const Instance& instance) override;
};

scip::ObjSepa* SubtourSeparatorTest::getSeparator(SimpleProgram& program,
                                                  const Instance& instance)
{
  return new SubtourSeparator(program);
}

TEST_F(SubtourSeparatorTest, testSubtourSeparator)
{
  testSeparator();
}
