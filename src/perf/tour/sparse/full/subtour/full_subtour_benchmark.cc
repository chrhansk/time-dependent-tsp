#include "full_subtour_benchmark.hh"

#include "tour/sparse/sparse_objective_propagator.hh"
#include "tour/sparse/pricers/sparse_path_router_pricer.hh"
#include "tour/sparse/heuristics/greedy_constructions.hh"

#include "tour/sparse/separators/sparse_separation_manager.hh"
#include "tour/sparse/separators/sparse_separators.hh"

SolutionResult FullSubtourBenchmark::execute(Instance& instance,
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

  separator->addSeparator(std::make_unique<SparseSubtourSeparator>(program));

  /*
    separator->addSeparator(std::make_unique<SparseCycleSeparator>(program));

    separator->addSeparator(std::make_unique<SparseOddPathFreeSeparator>(program));

    separator->addSeparator(std::make_unique<SparseUnitaryAFCSeparator>(program));
  */

  addSeparator(program, separator, instance);

  program.addSeparator(separator);

  program.addHeuristic(new CompoundGreedyConstruction(program));
  program.addHeuristic(new TimeGreedyConstruction(program));
  program.addHeuristic(new ValueGreedyConstruction(program));


  return program.solve(timeLimit);
}
