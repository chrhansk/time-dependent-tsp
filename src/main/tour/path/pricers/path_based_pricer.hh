#ifndef PATH_BASED_PRICER_HH
#define PATH_BASED_PRICER_HH

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <objscip/objscip.h>

#include "scip_utils.hh"

#include "graph/edge_set.hh"
#include "graph/graph.hh"

#include "tour/tour.hh"

#include "path_variable.hh"

#include "timed/timed_path.hh"
#include "timed/time_expanded_graph.hh"

#include "path_based_pricing_result.hh"

class PathBasedProgram;

const idx maxNumPaths = 20;

class PathBasedPricer : public scip::ObjPricer
{
protected:

  class ReducedCosts : public EdgeFunc<double>
  {
    const TimeExpandedGraph& graph;
    SimpleEdgeFunc<num> travelTimes;
    EdgeMap<double> dualValues;

  public:
    ReducedCosts(const TimeExpandedGraph& graph,
                 const EdgeFunc<double>& dualValues)
      : graph(graph),
        travelTimes(graph.travelTimes()),
        dualValues(graph, dualValues)
    {}

    double operator()(const Edge& edge) const override
    {
      return travelTimes(edge) - dualValues(edge);
    }
  };

  class FarkasCosts : public EdgeFunc<double>
  {
    const TimeExpandedGraph& graph;
    EdgeMap<double> farkasCosts;

  public:
    FarkasCosts(const TimeExpandedGraph& graph,
                const EdgeFunc<double>& farkasCosts)
      : graph(graph),
        farkasCosts(graph, farkasCosts)
    {}

    double operator()(const Edge& edge) const override
    {
      return -1. * farkasCosts(edge);
    }
  };

  PathBasedProgram& program;
  const idx maxNumPaths;

  std::string name;

  const Graph& originalGraph;
  const TimeExpandedGraph& graph;

  SCIP* scip;

  EdgeMap<SCIP_CONS*> linkingConstraints;
  idx timeHorizon;
  Vertex originalSource;

  TimedDistanceFunc& distances;

  EdgeMap<SCIP_VAR*> combinedVariables;
  std::vector<PathVariable> pathVariables;

  EdgeMap<double> getDualValues(DualCostType costType) const;

  const EdgeMap<SCIP_VAR*>& getCombinedVariables() const;

  PathBasedPricingResult performPricing(const EdgeFunc<double>& costs);

  virtual PathBasedPricingResult performPricing(DualCostType costType);

  void addSolution(const PathVariable& pathVariable,
                   const Path& path) const;

  void addTimedPaths(const std::vector<TimedPath>& timedPaths);

  void addResult(const PathBasedPricingResult& result,
                 double* lowerBound);

public:
  PathBasedPricer(PathBasedProgram& program,
                  const std::string& name,
                  idx numPaths = 20);

  virtual ~PathBasedPricer();

  virtual std::vector<TimedPath> findPaths(const EdgeFunc<double>& costs,
                                           const EdgeSet& forbiddenEdges,
                                           const std::optional<double>& lowerTimeBound,
                                           const std::optional<double>& upperTimeBound,
                                           std::optional<double>& minReducedCost) = 0;

  const std::string& getName() const
  {
    return name;
  }

  virtual SCIP_DECL_PRICERINIT(scip_init) override;

  virtual SCIP_DECL_PRICERREDCOST(scip_redcost) override;

  virtual SCIP_DECL_PRICERFARKAS(scip_farkas) override;

  virtual SCIP_DECL_PRICEREXIT(scip_exit) override;

  void printPaths(SCIP_SOL* sol) const;

  void addTour(const Tour& tour, bool pricing);

  /**
   * Adds a path leading through the time expanded graph
   **/
  virtual PathVariable addTimedPath(const TimedPath& timedPath,
                                    bool pricing = false);

  const std::vector<PathVariable>& getPathVariables() const
  {
    return pathVariables;
  }

  void releaseVariables();

};

#endif /* PATH_BASED_PRICER_HH */
