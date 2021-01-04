#include "sparse_separator_test.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_lifted_subtour_separator.hh"

class SparseLiftedSubtourSeparatorTest : public SparseSeparatorTest
{
protected:
  virtual void addSeparator(SparseProgram& program,
                            const Instance& instance) override;

};

void SparseLiftedSubtourSeparatorTest::addSeparator(SparseProgram& program,
                                                 const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseLiftedSubtourSeparator>(program,
                                                                         instance.staticCosts));

  program.addSeparator(separator);
}

TEST_F(SparseLiftedSubtourSeparatorTest, testSparseLiftedSubtourSeparator)
{
  testSeparator();
}
