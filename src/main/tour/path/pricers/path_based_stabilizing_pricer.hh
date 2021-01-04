#ifndef PATH_BASED_STABILIZING_PRICER_HH
#define PATH_BASED_STABILIZING_PRICER_HH

#include "path_based_router_pricer.hh"


template<class Router>
class PathBasedStabilizingPricer : public PathBasedRouterPricer<Router>
{
private:

  class DualValues : public EdgeFunc<double>
  {
  private:
    std::optional<double> lagrangianBound;
    double offset;
    EdgeMap<double> dualValues;

  public:
    DualValues(const EdgeMap<double>& dualValues,
               std::optional<double> lagrangianBound = 0.,
               double offset = 0.)
      : lagrangianBound(lagrangianBound),
        offset(offset),
        dualValues(dualValues)
    {}

    DualValues(const DualValues& other) = default;

    DualValues& operator=(const DualValues& other) = default;

    double operator()(const Edge& edge) const override
    {
      return dualValues(edge);
    }

    double getOffset() const
    {
      return offset;
    }

    void setOffset(double value)
    {
      offset = value;
    }

    std::optional<double> getLagrangianBound() const
    {
      return lagrangianBound;
    }

    void setLagrangianBound(double value)
    {
      lagrangianBound = value;
    }
  };

  bool proveDirectly;
  const bool proveOptimality;
  const double explorationFactor;

  const double initialProveGap;
  double proveGap;

  const std::optional<double> cutoffThreshold;

  DualValues centeredDualValues;

  DualValues getCurrentDualValues();

  using PathBasedRouterPricer<Router>::graph;
  using PathBasedRouterPricer<Router>::originalGraph;
  using PathBasedRouterPricer<Router>::program;

  using PathBasedRouterPricer<Router>::findPaths;

  using PathBasedRouterPricer<Router>::ReducedCosts;

  using PathBasedRouterPricer<Router>::getDualValues;
  using PathBasedRouterPricer<Router>::getPathVariables;
  using PathBasedRouterPricer<Router>::scip;

protected:
  virtual PathBasedPricingResult performPricing(DualCostType costType) override;

  void checkDualValues(const DualValues& dualValues) const;

  std::vector<TimedPath> findPaths(const EdgeFunc<double>& dualValues,
                                   double& minReducedCost);

public:
  PathBasedStabilizingPricer(PathBasedProgram& program,
                             bool proveOptimality = true)
    : PathBasedRouterPricer<Router>(program),
      proveDirectly(false),
      proveOptimality(proveOptimality),
      explorationFactor(0.1),
      initialProveGap(0.001),
      proveGap(initialProveGap),
      cutoffThreshold(1e-4),
      centeredDualValues(EdgeMap<double>(graph, 0.), 0., 0.)
  {}

};

template<class Router>
typename PathBasedStabilizingPricer<Router>::DualValues
PathBasedStabilizingPricer<Router>::getCurrentDualValues()
{
  EdgeMap<double> dualValues = getDualValues(DualCostType::SIMPLE);

  const double upperBound = SCIPgetLPObjval(scip);

  return DualValues(dualValues, 0., upperBound);
}

template<class Router>
std::vector<TimedPath>
PathBasedStabilizingPricer<Router>::findPaths(const EdgeFunc<double>& dualValues,
                                              double& minReducedCost)
{
  EdgeSet forbiddenEdges(originalGraph);
  const std::optional<double> lowerTimeBound;
  const std::optional<double> upperTimeBound;

  std::optional<double> minCost;

  typename PathBasedRouterPricer<Router>::ReducedCosts reducedCosts(graph, dualValues);

  auto paths = findPaths(reducedCosts,
                         forbiddenEdges,
                         lowerTimeBound,
                         upperTimeBound,
                         minCost);

  if(minCost)
  {
    minReducedCost = *minCost;
  }
  else
  {
    assert(paths.empty());
    minReducedCost = 0.;
  }

  assert(minReducedCost <= 0.);

  return paths;
}

template<class Router> void
PathBasedStabilizingPricer<Router>::checkDualValues(const typename PathBasedStabilizingPricer<Router>::DualValues& dualValues) const
{
  typename PathBasedRouterPricer<Router>::ReducedCosts reducedCosts(graph, dualValues);

  for(const PathVariable& pathVariable : getPathVariables())
  {
    const double reducedCost = pathVariable.getTimedPath().cost(reducedCosts);

    assert(cmp::ge(reducedCost, 0.));
  }
}

template<class Router>
PathBasedPricingResult PathBasedStabilizingPricer<Router>::performPricing(DualCostType costType)
{
  if(costType == DualCostType::FARKAS)
  {
    return PathBasedRouterPricer<Router>::performPricing(costType);
  }

  if(program.hasFixedEdges() || proveDirectly)
  {
    return PathBasedRouterPricer<Router>::performPricing(costType);
  }

  // separator check at some point

  const DualValues currentDualValues = getCurrentDualValues();

  if(debuggingEnabled())
  {
    checkDualValues(currentDualValues);
  }

  bool misPriced = false;
  const double upperBound = SCIPgetLPObjval(scip);

  Log(info) << "Performing stabilized reduced cost pricing";

  assert(centeredDualValues.getLagrangianBound());

  auto centeredBound = [&]() -> double
    {
      return *centeredDualValues.getLagrangianBound();
    };


  do
  {
    bool movedCenter = false;

    assert(cmp::ge(upperBound, centeredBound()));

    if(proveOptimality)
    {
      const double gap = (upperBound - centeredBound()) / centeredBound();

      if(gap <= proveGap)
      {
        Log(info) << "Trying to prove optimality (gap: "
                  << std::fixed
                  << std::setprecision(2)
                  << 100*gap
                  << "%)";

        double minReducedCost;

        const std::vector<TimedPath> currentPaths = findPaths(currentDualValues,
                                                              minReducedCost);


        if(currentPaths.empty())
        {
          Log(info) << "Successfully proved optimality";
        }
        else
        {
          Log(info) << "Failed to prove optimality";

          auto currentLowerBound = upperBound + minReducedCost;

          if(cmp::gt(currentLowerBound, centeredBound()))
          {
            Log(info) << "Bound improved to "
                      << currentLowerBound
                      << ", moving center";

            centeredDualValues = currentDualValues;

            centeredDualValues.setLagrangianBound(currentLowerBound);

            movedCenter = true;
          }
          else
          {
            proveGap /= 2.;
          }


          Log(info) << "Resolving master problem with "
                    << currentPaths.size()
                    << " additional paths";
        }

        return PathBasedPricingResult(currentPaths, centeredBound());
      }
    }

    EdgeMap<double> values(graph, 0.);

    for(const Edge& edge : graph.getEdges())
    {
      values(edge) = explorationFactor*currentDualValues(edge) +
        (1 - explorationFactor)*centeredDualValues(edge);
    }

    double minReducedCosts;

    DualValues stabilizedDualValues(values, 0.);

    std::vector<TimedPath> stabilizedPaths = findPaths(stabilizedDualValues, minReducedCosts);

    stabilizedDualValues.setOffset(explorationFactor*upperBound +
                                   (1 - explorationFactor)*centeredDualValues.getOffset());

    const double stabilizedBound = stabilizedDualValues.getOffset() + minReducedCosts;

    stabilizedDualValues.setLagrangianBound(stabilizedBound);

    assert(cmp::ge(upperBound, stabilizedBound));

    // Test that the Lagrangean bounds are concave
    if(debuggingEnabled())
    {
      double currentMinReducedCosts;

      findPaths(currentDualValues, currentMinReducedCosts);

      const double currentBound = upperBound + currentMinReducedCosts;

      assert(cmp::ge(stabilizedBound,
                     explorationFactor*currentBound +
                     (1 - explorationFactor)*centeredBound()));
    }

    std::vector<TimedPath> currentPaths;

    {
      misPriced = true;

      for(const TimedPath& path : stabilizedPaths)
      {
        typename PathBasedRouterPricer<Router>::ReducedCosts
          currentReducedCosts(graph, currentDualValues);

        const double pathReducedCost = path.cost(currentReducedCosts);

        if(cmp::neg(pathReducedCost))
        {
          currentPaths.push_back(path);
          misPriced = false;
        }
      }
    }

    if(cmp::gt(*stabilizedDualValues.getLagrangianBound(),
               centeredBound()))
    {
      Log(info) << "Bound improved to "
                << *stabilizedDualValues.getLagrangianBound()
                << ", moving center";

      centeredDualValues = stabilizedDualValues;

      movedCenter = true;
    }
    else
    {
      Log(info) << "Stabilized bound did not improve";
    }

    const double gap = (upperBound - centeredBound()) / centeredBound();

    Log(info) << "Lower bound: "
              << centeredBound()
              << ", upper bound: "
              << upperBound
              << ", gap: "
              << std::fixed
              << std::setprecision(2)
              << 100*gap
              << "%";

    if(cutoffThreshold && upperBound <= centeredBound() + *cutoffThreshold)
    {
      Log(info) << "Achieved optimum solution value";
      break;
    }

    if(misPriced)
    {
      if(movedCenter)
      {
        Log(info) << "Misprice. Continuing";
      }
      else
      {
        Log(info) << "Falling back to default pricing";
        proveDirectly = true;
      }
    }
    else
    {
      Log(info) << "Resolving master problem with "
                << currentPaths.size()
                << " new paths";

      return PathBasedPricingResult(currentPaths, centeredBound());
    }
  }
  while(misPriced && !proveDirectly);

  if(proveDirectly)
  {
    return PathBasedRouterPricer<Router>::performPricing(costType);
  }

  return PathBasedPricingResult::empty();
}


typedef PathBasedStabilizingPricer<SimpleTimeExpandedRouter> PathBasedSimpleStabilizingPricer;

typedef PathBasedStabilizingPricer<TwoCycleFreeTimeExpandedRouter> PathBasedTwoCycleFreeStabilizingPricer;

template <idx size>
using PathBasedAcyclicStabilizingPricer = PathBasedStabilizingPricer<AcyclicTimeExpandedRouter<size>>;

template <idx size>
using PathBasedAcyclicHoleFreeStabilizingPricer = PathBasedStabilizingPricer<AcyclicHoleFreeTimeExpandedRouter<size>>;


#endif /* PATH_BASED_STABILIZING_PRICER_HH */
