#include "full_subtour_odd_path_free_benchmark.hh"

#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_separators.hh"

void FullSubtourOddPathFreeBenchmark::addSeparator(SparseProgram& program,
                                                         SparseSeparationManager* separator,
                                                         const Instance& instance)
{
  separator->addSeparator(std::make_unique<SparseOddPathFreeSeparator>(program));
}

int main(int argc, char *argv[])
{
  FullSubtourOddPathFreeBenchmark().run(argc, argv);

  return 0;
}
