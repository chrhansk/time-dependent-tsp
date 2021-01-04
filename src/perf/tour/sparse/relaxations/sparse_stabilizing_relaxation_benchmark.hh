#ifndef SPARSE_STABILIZING_RELAXATION_BENCHMARK_HH
#define SPARSE_STABILIZING_RELAXATION_BENCHMARK_HH

#include "tour/relaxation_benchmark.hh"

#include "tour/sparse/pricers/sparse_pricer.hh"
#include "tour/sparse/pricers/sparse_stabilizing_pricer.hh"

template <class Pricer>
class SparseStabilizingRelaxationBenchmark : public RelaxationBenchmark
{
protected:
  virtual SolutionResult execute(Instance& instance,
                                 const Tour& initialTour,
                                 int timeLimit = -1) override;

  virtual SparseStabilizingPricer* getPricer(SparseProgram& program) const;
};


template <class Pricer>
SolutionResult SparseStabilizingRelaxationBenchmark<Pricer>::execute(Instance& instance,
                                                          const Tour& initialTour,
                                                          int timeLimit)
{
  SparseProgram program(initialTour,
                        instance.timedDistances,
                        0.,
                        false,
                        Program::Settings().collectStats().doSolveRelaxation());

  program.setPricer(getPricer(program));

  return program.solve(timeLimit);
}

template <class Pricer>
SparseStabilizingPricer* SparseStabilizingRelaxationBenchmark<Pricer>::getPricer(SparseProgram& program) const
{
  auto pricer = std::make_unique<Pricer>(program);

  return new SparseStabilizingPricer(program,
                                     std::move(pricer));
}


#endif /* SPARSE_STABILIZING_RELAXATION_BENCHMARK_HH */
