#include "sparse_separator_test.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_subtour_separator.hh"

class SparseSubtourSeparatorTest : public SparseSeparatorTest
{
protected:
  virtual void addSeparator(SparseProgram& program,
                            const Instance& instance) override;

};

void SparseSubtourSeparatorTest::addSeparator(SparseProgram& program,
                                              const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseSubtourSeparator>(program));

  program.addSeparator(separator);
}

TEST_F(SparseSubtourSeparatorTest, testSparseSubtourSeparator)
{
  testSeparator();
}
