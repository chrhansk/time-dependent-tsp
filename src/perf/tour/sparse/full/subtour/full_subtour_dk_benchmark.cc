#include "full_subtour_dk_benchmark.hh"

#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_separators.hh"

void FullSubtourDKBenchmark::addSeparator(SparseProgram& program,
                                                SparseSeparationManager* separator,
                                                const Instance& instance)
{
  separator->addSeparator(std::make_unique<SparseDKSeparator>(program));
}

int main(int argc, char *argv[])
{
  FullSubtourDKBenchmark().run(argc, argv);

  return 0;
}
