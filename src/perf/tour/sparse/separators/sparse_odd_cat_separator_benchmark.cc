#include "sparse_separator_benchmark.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_odd_cat_separator.hh"

class SparseOddCATSeparatorBenchmark : public SparseSeparatorBenchmark
{
protected:
  virtual void addSeparator(SparseProgram& program,
                            const Instance& instance) override;

};

void SparseOddCATSeparatorBenchmark::addSeparator(SparseProgram& program,
                                                  const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseOddCATSeparator>(program));

  program.addSeparator(separator);
}

int main(int argc, char *argv[])
{
  SparseOddCATSeparatorBenchmark().run(argc, argv);

  return 0;
}
