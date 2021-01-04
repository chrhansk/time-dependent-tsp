#ifndef SPARSE_RELAXATION_BENCHMARK_HH
#define SPARSE_RELAXATION_BENCHMARK_HH

#include "tour/relaxation_benchmark.hh"

#include "tour/sparse/pricers/sparse_pricer.hh"

template <class Pricer>
class SparseRelaxationBenchmark : public RelaxationBenchmark
{
protected:
  virtual SolutionResult execute(Instance& instance,
                                 const Tour& initialTour,
                                 int timeLimit = -1) override;

  virtual Pricer* getPricer(SparseProgram& program) const;
};


template <class Pricer>
SolutionResult SparseRelaxationBenchmark<Pricer>::execute(Instance& instance,
                                                          const Tour& initialTour,
                                                          int timeLimit)
{
  SparseProgram program(initialTour,
                        instance.timedDistances,
                        0.,
                        false,
                        Program::Settings().collectStats().doSolveRelaxation());

  program.setPricer(getPricer(program));

  return program.solveRelaxation(timeLimit);
}

template <class Pricer>
Pricer* SparseRelaxationBenchmark<Pricer>::getPricer(SparseProgram& program) const
{
  return new Pricer(program);
}


#endif /* SPARSE_RELAXATION_BENCHMARK_HH */
