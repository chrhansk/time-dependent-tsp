#ifndef FULL_LIFTED_SUBTOUR_ODD_PATH_FREE_BENCHMARK_HH
#define FULL_LIFTED_SUBTOUR_ODD_PATH_FREE_BENCHMARK_HH

#include "full_subtour_benchmark.hh"

class FullSubtourOddPathFreeBenchmark : public FullSubtourBenchmark
{
public:
  void addSeparator(SparseProgram& program,
                    SparseSeparationManager* separator,
                    const Instance& instance) override;
};

#endif /* FULL_LIFTED_SUBTOUR_ODD_PATH_FREE_BENCHMARK_HH */
