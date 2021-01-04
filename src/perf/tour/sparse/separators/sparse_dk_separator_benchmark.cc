#include "sparse_separator_benchmark.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_dk_separator.hh"

class SparseDKSeparatorBenchmark : public SparseSeparatorBenchmark
{
protected:
  virtual void addSeparator(SparseProgram& program,
                            const Instance& instance) override;

};

void SparseDKSeparatorBenchmark::addSeparator(SparseProgram& program,
                                              const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseDKSeparator>(program));

  program.addSeparator(separator);
}

int main(int argc, char *argv[])
{
  SparseDKSeparatorBenchmark().run(argc, argv);

  return 0;
}
