#include "sparse_separator_benchmark.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_unitary_afc_separator.hh"

class SparseUnitaryAFCSeparatorBenchmark : public SparseSeparatorBenchmark
{
protected:
  virtual void addSeparator(SparseProgram& program,
                            const Instance& instance) override;

};

void SparseUnitaryAFCSeparatorBenchmark::addSeparator(SparseProgram& program,
                                                      const Instance& instance)
{
  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseUnitaryAFCSeparator>(program));

  program.addSeparator(separator);
}

int main(int argc, char *argv[])
{
  SparseUnitaryAFCSeparatorBenchmark().run(argc, argv);

  return 0;
}
