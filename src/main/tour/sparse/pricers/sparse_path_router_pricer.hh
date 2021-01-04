#ifndef SPARSE_PATH_ROUTER_PRICER_HH
#define SPARSE_PATH_ROUTER_PRICER_HH

#include "sparse_path_pricer.hh"

#include "timed/router/simple_time_expanded_router.hh"
#include "timed/router/two_cycle_free_time_expanded_router.hh"
#include "timed/router/acyclic_hole_free_time_expanded_router.hh"
#include "timed/router/acyclic_time_expanded_router.hh"

template <class Router>
class SparsePathRouterPricer : public SparsePathPricer
{
public:
  SparsePathRouterPricer(SparseProgram& program)
    : SparsePathPricer(program)
  {}

  std::vector<TimedPath>
  findPaths(const EdgeFunc<double>& reducedCosts,
            const EdgeSet& forbiddenEdges,
            const std::optional<double>& lowerTimeBound,
            const std::optional<double>& upperTimeBound,
            std::optional<double>& minReducedCost)
  {
    Router router(graph, source);

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

};

typedef SparsePathRouterPricer<SimpleTimeExpandedRouter> SparseSimplePathPricer;

typedef SparsePathRouterPricer<TwoCycleFreeTimeExpandedRouter> SparseTwoCycleFreePricer;

template <idx size>
using SparseAcyclicPricer = SparsePathRouterPricer<AcyclicTimeExpandedRouter<size>>;

template <idx size>
using SparseAcyclicHoleFreePricer = SparsePathRouterPricer<AcyclicHoleFreeTimeExpandedRouter<size>>;


#endif /* SPARSE_PATH_ROUTER_PRICER_HH */
