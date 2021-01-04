#include "sparse_separator_test.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_cycle_separator.hh"

class SparseCycleSeparatorTest : public SparseSeparatorTest
{
protected:
  virtual void addSeparator(SparseProgram& program,
                            const Instance& instance) override;

};

void SparseCycleSeparatorTest::addSeparator(SparseProgram& program,
                                              const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseCycleSeparator>(program));

  program.addSeparator(separator);
}

TEST_F(SparseCycleSeparatorTest, testSparseCycleSeparator)
{
  testSeparator();
}
