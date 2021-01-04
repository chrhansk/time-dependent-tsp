#ifndef PATH_BASED_ROUTER_PRICER_HH
#define PATH_BASED_ROUTER_PRICER_HH

#include "path_based_pricer.hh"

#include "timed/router/simple_time_expanded_router.hh"
#include "timed/router/acyclic_time_expanded_router.hh"
#include "timed/router/acyclic_hole_free_time_expanded_router.hh"
#include "timed/router/two_cycle_free_time_expanded_router.hh"

template<class Router>
class PathBasedRouterPricer : public PathBasedPricer
{
public:
  PathBasedRouterPricer(PathBasedProgram& program)
    : PathBasedPricer(program, "path_based_router_pricer")
  {}

  std::vector<TimedPath> findPaths(const EdgeFunc<double>& reducedCosts,
                                   const EdgeSet& forbiddenEdges,
                                   const std::optional<double>& lowerTimeBound,
                                   const std::optional<double>& upperTimeBound,
                                   std::optional<double>& minReducedCost) override
  {
    Router router(graph, originalSource);

    TimeExpandedRouter::Request request(reducedCosts,
                                        forbiddenEdges);

    request.cutoffCost = 0.;
    request.maxNumPaths = maxNumPaths;

    request.lowerTimeBound = lowerTimeBound;
    request.upperTimeBound = upperTimeBound;

    auto result = router.findShortestPaths(request);

    minReducedCost = result.minCost;

    return result.paths;
  }
};

typedef PathBasedRouterPricer<SimpleTimeExpandedRouter> PathBasedSimplePricer;

typedef PathBasedRouterPricer<TwoCycleFreeTimeExpandedRouter> PathBasedTwoCycleFreePricer;

template <idx size>
using PathBasedAcyclicPricer = PathBasedRouterPricer<AcyclicTimeExpandedRouter<size>>;

template <idx size>
using PathBasedAcyclicHoleFreePricer = PathBasedRouterPricer<AcyclicHoleFreeTimeExpandedRouter<size>>;

#endif /* PATH_BASED_ROUTER_PRICER_HH */
