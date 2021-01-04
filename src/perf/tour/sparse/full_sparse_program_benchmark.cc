#include "tour/program_benchmark.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/sparse_objective_propagator.hh"
#include "tour/sparse/pricers/sparse_path_router_pricer.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_separators.hh"
#include "tour/sparse/heuristics/greedy_constructions.hh"


class FullSparseProgramBenchmark : public ProgramBenchmark
{
  virtual SolutionResult execute(Instance& instance,
                                 const Tour& initialTour,
                                 int timeLimit = -1) override;
};

SolutionResult FullSparseProgramBenchmark::execute(Instance& instance,
                                                   const Tour& initialTour,
                                                   int timeLimit)
{
  SparseProgram program(initialTour,
                        instance.timedDistances,
                        0,
                        false,
                        Program::Settings().collectStats());

  program.setPricer(new SparseTwoCycleFreePricer(program));

  program.addPropagator(new SparseObjectivePropagator(program));

  SparseSeparationManager* separator = new SparseSeparationManager(program);

  separator->addSeparator(std::make_unique<SparseLiftedSubtourSeparator>(program,
                                                                         instance.staticCosts));

  separator->addSeparator(std::make_unique<SparseDKSeparator>(program));

  /*
  separator->addSeparator(std::make_unique<SparseCycleSeparator>(program));

  separator->addSeparator(std::make_unique<SparseOddPathFreeSeparator>(program));

  separator->addSeparator(std::make_unique<SparseUnitaryAFCSeparator>(program));
  */

  program.addSeparator(separator);

  program.addHeuristic(new CompoundGreedyConstruction(program));
  program.addHeuristic(new TimeGreedyConstruction(program));
  program.addHeuristic(new ValueGreedyConstruction(program));


  return program.solve(timeLimit);
}

int main(int argc, char *argv[])
{
  FullSparseProgramBenchmark().run(argc, argv);

  return 0;
}
