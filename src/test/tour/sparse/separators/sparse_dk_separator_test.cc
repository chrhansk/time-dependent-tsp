#include "sparse_separator_test.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_dk_separator.hh"

class SparseDKSeparatorTest : public SparseSeparatorTest
{
protected:
  virtual void addSeparator(SparseProgram& program,
                            const Instance& instance) override;

};

void SparseDKSeparatorTest::addSeparator(SparseProgram& program,
                                         const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseDKSeparator>(program));

  program.addSeparator(separator);
}

TEST_F(SparseDKSeparatorTest, testSparseDKSeparator)
{
  testSeparator();
}
