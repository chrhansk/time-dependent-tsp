#include "combined_separator_benchmark.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_odd_cat_separator.hh"
#include "tour/sparse/separators/sparse_lifted_subtour_separator.hh"

class CombinedOddCATSeparatorBenchmark : public CombinedSeparatorBenchmark
{
protected:
  virtual void addSeparators(SparseProgram& program,
                             const Instance& instance) override;

};

void CombinedOddCATSeparatorBenchmark::addSeparators(SparseProgram& program,
                                                     const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseLiftedSubtourSeparator>(program,
                                                                         instance.staticCosts));

  separator->addSeparator(std::make_unique<SparseOddCATSeparator>(program));

  program.addSeparator(separator);
}

int main(int argc, char *argv[])
{
  CombinedOddCATSeparatorBenchmark().run(argc, argv);

  return 0;
}
