#include "sparse_separator_benchmark.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_odd_path_free_separator.hh"

class SparseOddPathFreeSeparatorBenchmark : public SparseSeparatorBenchmark
{
protected:
  virtual void addSeparator(SparseProgram& program,
                            const Instance& instance) override;

};

void SparseOddPathFreeSeparatorBenchmark::addSeparator(SparseProgram& program,
                                                          const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseOddPathFreeSeparator>(program));

  program.addSeparator(separator);
}

int main(int argc, char *argv[])
{
  SparseOddPathFreeSeparatorBenchmark().run(argc, argv);

  return 0;
}
