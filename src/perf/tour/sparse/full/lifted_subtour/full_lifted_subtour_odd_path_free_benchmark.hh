#ifndef FULL_LIFTED_SUBTOUR_ODD_PATH_FREE_BENCHMARK_HH
#define FULL_LIFTED_SUBTOUR_ODD_PATH_FREE_BENCHMARK_HH

#include "full_lifted_subtour_benchmark.hh"

class FullLiftedSubtourOddPathFreeBenchmark : public FullLiftedSubtourBenchmark
{
public:
  void addSeparator(SparseProgram& program,
                    SparseSeparationManager* separator,
                    const Instance& instance) override;
};

#endif /* FULL_LIFTED_SUBTOUR_ODD_PATH_FREE_BENCHMARK_HH */
