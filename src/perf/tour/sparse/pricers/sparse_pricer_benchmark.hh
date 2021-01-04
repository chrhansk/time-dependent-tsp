#include "tour/sparse/sparse_program_benchmark.hh"

#include "tour/sparse/sparse_program.hh"

class SparsePricerBenchmark : public SparseProgramBenchmark
{
public:
  SolutionResult execute(Instance& instance,
                         const Tour& initialTour,
                         int timeLimit) override;

  virtual SparsePricer* getPricer(SparseProgram& program) const = 0;
};
