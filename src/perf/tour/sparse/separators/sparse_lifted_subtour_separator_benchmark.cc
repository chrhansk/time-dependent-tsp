#include "sparse_separator_benchmark.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_lifted_subtour_separator.hh"

class SparseLiftedSubtourSeparatorBenchmark : public SparseSeparatorBenchmark
{
protected:
  virtual void addSeparator(SparseProgram& program,
                            const Instance& instance) override;

};

void SparseLiftedSubtourSeparatorBenchmark::addSeparator(SparseProgram& program,
                                                         const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseLiftedSubtourSeparator>(program,
                                                                         instance.staticCosts));

  program.addSeparator(separator);
}

int main(int argc, char *argv[])
{
  SparseLiftedSubtourSeparatorBenchmark().run(argc, argv);

  return 0;
}
