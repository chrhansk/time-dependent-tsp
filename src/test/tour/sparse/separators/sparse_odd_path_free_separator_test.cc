#include "sparse_separator_test.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_odd_path_free_separator.hh"

class SparseOddPathFreeSeparatorTest : public SparseSeparatorTest
{
protected:
  virtual void addSeparator(SparseProgram& program,
                            const Instance& instance) override;

};

void SparseOddPathFreeSeparatorTest::addSeparator(SparseProgram& program,
                                                     const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseOddPathFreeSeparator>(program));

  program.addSeparator(separator);
}

TEST_F(SparseOddPathFreeSeparatorTest, testSparseOddPathFreeSeparator)
{
  testSeparator();
}
