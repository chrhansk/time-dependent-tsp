#include "sparse_separator_benchmark.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_subtour_separator.hh"

class SparseSubtourSeparatorBenchmark : public SparseSeparatorBenchmark
{
protected:
  virtual void addSeparator(SparseProgram& program,
                            const Instance& instance) override;

};

void SparseSubtourSeparatorBenchmark::addSeparator(SparseProgram& program,
                                                   const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseSubtourSeparator>(program));

  program.addSeparator(separator);
}

int main(int argc, char *argv[])
{
  SparseSubtourSeparatorBenchmark().run(argc, argv);

  return 0;
}
