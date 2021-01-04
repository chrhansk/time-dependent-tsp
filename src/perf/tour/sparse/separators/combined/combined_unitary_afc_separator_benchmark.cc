#include "combined_separator_benchmark.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_unitary_afc_separator.hh"
#include "tour/sparse/separators/sparse_lifted_subtour_separator.hh"

class CombinedUnitaryAFCSeparatorBenchmark : public CombinedSeparatorBenchmark
{
protected:
  virtual void addSeparators(SparseProgram& program,
                             const Instance& instance) override;

};

void CombinedUnitaryAFCSeparatorBenchmark::addSeparators(SparseProgram& program,
                                                         const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseLiftedSubtourSeparator>(program,
                                                                         instance.staticCosts));

  separator->addSeparator(std::make_unique<SparseUnitaryAFCSeparator>(program));

  program.addSeparator(separator);
}

int main(int argc, char *argv[])
{
  CombinedUnitaryAFCSeparatorBenchmark().run(argc, argv);

  return 0;
}
