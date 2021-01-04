#ifndef PATH_BASED_RELAXATION_BENCHMARK_HH
#define PATH_BASED_RELAXATION_BENCHMARK_HH

#include "tour/relaxation_benchmark.hh"

#include "tour/sparse/pricers/sparse_pricer.hh"

#include "tour/path/path_based_program.hh"
#include "tour/path/pricers/path_based_pricer.hh"

template <class Pricer>
class PathBasedRelaxationBenchmark : public RelaxationBenchmark
{
protected:
  virtual SolutionResult execute(Instance& instance,
                                 const Tour& initialTour,
                                 int timeLimit = -1) override;

  virtual Pricer* getPricer(PathBasedProgram& program) const;  
};


template <class Pricer>
SolutionResult PathBasedRelaxationBenchmark<Pricer>::execute(Instance& instance,
                                                             const Tour& initialTour,
                                                             int timeLimit)
{
  PathBasedProgram program(initialTour,
                        instance.timedDistances,
                        0.,
                        false,
                        Program::Settings().collectStats().doSolveRelaxation());

  program.setPricer(getPricer(program));

  return program.solveRelaxation(timeLimit);
}

template <class Pricer>
Pricer* PathBasedRelaxationBenchmark<Pricer>::getPricer(PathBasedProgram& program) const
{
  return new Pricer(program);
}


#endif /* PATH_BASED_RELAXATION_BENCHMARK_HH */
