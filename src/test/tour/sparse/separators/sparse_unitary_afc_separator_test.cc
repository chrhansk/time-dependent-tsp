#include "sparse_separator_test.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_unitary_afc_separator.hh"

class SparseUnitaryAFCSeparatorTest : public SparseSeparatorTest
{
protected:
  virtual void addSeparator(SparseProgram& program,
                            const Instance& instance) override;

};

void SparseUnitaryAFCSeparatorTest::addSeparator(SparseProgram& program,
                                              const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseUnitaryAFCSeparator>(program));

  program.addSeparator(separator);
}

TEST_F(SparseUnitaryAFCSeparatorTest, testSparseUnitaryAFCSeparator)
{
  testSeparator();
}
