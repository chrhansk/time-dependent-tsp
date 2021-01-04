#include "full_subtour_unitary_afc_benchmark.hh"

#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_separators.hh"

void FullSubtourUnitaryAfcBenchmark::addSeparator(SparseProgram& program,
                                                        SparseSeparationManager* separator,
                                                        const Instance& instance)
{
  separator->addSeparator(std::make_unique<SparseUnitaryAFCSeparator>(program));
}

int main(int argc, char *argv[])
{
  FullSubtourUnitaryAfcBenchmark().run(argc, argv);

  return 0;
}
