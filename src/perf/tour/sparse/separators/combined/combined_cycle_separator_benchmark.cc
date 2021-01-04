#include "combined_separator_benchmark.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_cycle_separator.hh"
#include "tour/sparse/separators/sparse_lifted_subtour_separator.hh"

class CombinedCycleSeparatorBenchmark : public CombinedSeparatorBenchmark
{
protected:
  virtual void addSeparators(SparseProgram& program,
                            const Instance& instance) override;

};

void CombinedCycleSeparatorBenchmark::addSeparators(SparseProgram& program,
                                                  const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseLiftedSubtourSeparator>(program,
                                                                         instance.staticCosts));

  separator->addSeparator(std::make_unique<SparseCycleSeparator>(program));

  program.addSeparator(separator);
}

int main(int argc, char *argv[])
{
  CombinedCycleSeparatorBenchmark().run(argc, argv);

  return 0;
}
