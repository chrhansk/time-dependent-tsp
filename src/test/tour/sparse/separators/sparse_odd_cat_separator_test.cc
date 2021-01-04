#include "sparse_separator_test.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_odd_cat_separator.hh"

class SparseOddCATSeparatorTest : public SparseSeparatorTest
{
protected:
  virtual void addSeparator(SparseProgram& program,
                            const Instance& instance) override;

};

void SparseOddCATSeparatorTest::addSeparator(SparseProgram& program,
                                             const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseOddCATSeparator>(program));

  program.addSeparator(separator);
}

TEST_F(SparseOddCATSeparatorTest, testSparseOddCATSeparator)
{
  testSeparator();
}
