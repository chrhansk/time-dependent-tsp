#ifndef SPARSE_ACYCLIC_PRICER_HH
#define SPARSE_ACYCLIC_PRICER_HH

#include "sparse_path_pricer.hh"

#include "timed/router/acyclic_time_expanded_router.hh"

template<idx size>
class SparseAcyclicPricer : public SparsePathPricer
{
private:
  TimedVertex timedSource;

public:
  SparseAcyclicPricer(SparseProgram& program);

  virtual std::vector<TimedPath> findPaths(const EdgeFunc<double>& reducedCosts,
                                           const EdgeSet& forbiddenEdges,
                                           const std::optional<double>& lowerTimeBound,
                                           const std::optional<double>& upperTimeBound,
                                           std::optional<double>& minReducedCost) override;
};

template<idx size>
SparseAcyclicPricer<size>::SparseAcyclicPricer(SparseProgram& program)
  : SparsePathPricer(program),
    timedSource(graph.getVertex(source, 0))
{}

template<idx size>
std::vector<TimedPath>
SparseAcyclicPricer<size>::findPaths(const EdgeFunc<double>& reducedCosts,
                                     const EdgeSet& forbiddenEdges,
                                     const std::optional<double>& lowerTimeBound,
                                     const std::optional<double>& upperTimeBound,
                                     std::optional<double>& minReducedCost)
{
  AcyclicTimeExpandedRouter<size> router(graph, source);

  TimeExpandedRouter::Request request(reducedCosts,
                                      forbiddenEdges);

  request.cutoffCost = 0.;
  request.maxNumPaths = numPaths;

  request.lowerTimeBound = lowerTimeBound;
  request.upperTimeBound = upperTimeBound;

  auto result = router.findShortestPaths(request);

  minReducedCost = result.minCost;

  return result.paths;
}


#endif /* SPARSE_ACYCLIC_PRICER_HH */
