#include "separator_test.hh"

#include "tour/timed/separators/simple_unitary_afc_separator.hh"

class SimpleUnitaryAFCSeparatorTest : public SeparatorTest
{
protected:
  virtual scip::ObjSepa* getSeparator(SimpleProgram& program,
                                      const Instance& instance) override;
};

scip::ObjSepa* SimpleUnitaryAFCSeparatorTest::getSeparator(SimpleProgram& program,
                                                           const Instance& instance)
{
  return new SimpleUnitaryAFCSeparator(program);
}

TEST_F(SimpleUnitaryAFCSeparatorTest, testUnitaryAFCSeparator)
{
  testSeparator();
}
