#ifndef FULL_LIFTED_SUBTOUR_UNITARY_AFC_BENCHMARK_HH
#define FULL_LIFTED_SUBTOUR_UNITARY_AFC_BENCHMARK_HH

#include "full_subtour_benchmark.hh"

class FullSubtourUnitaryAfcBenchmark : public FullSubtourBenchmark
{
public:
  void addSeparator(SparseProgram& program,
                    SparseSeparationManager* separator,
                    const Instance& instance) override;
};



#endif /* FULL_LIFTED_SUBTOUR_UNITARY_AFC_BENCHMARK_HH */
