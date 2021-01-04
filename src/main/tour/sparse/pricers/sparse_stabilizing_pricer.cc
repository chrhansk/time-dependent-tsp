#include "sparse_stabilizing_pricer.hh"

#include <iomanip>

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/separators/sparse_separation_manager.hh"

#include "tour/sparse/pricers/sparse_pricing_manager.hh"


const std::optional<double> cutoffThreshold;

const bool proveOptimality = true;

static_assert(proveOptimality || cutoffThreshold);

const double SparseStabilizingPricer::initialProveGap = 0.005;

const double explorationFactor = 0.1;

SparseStabilizingPricer::SparseStabilizingPricer(SparseProgram& program,
                                                 std::unique_ptr<SparsePathPricer>&& sparsePricer)
  : SparsePricer(program),
    sparsePricer(std::move(sparsePricer)),
    originalGraph(program.getGraph().underlyingGraph()),
    proveGap(initialProveGap),
    centeredDualValues(EdgeMap<double>(graph, 0.), 0., 0.)
{
}

std::vector<TimedPath>
SparseStabilizingPricer::findPaths(const EdgeFunc<double>& dualValues,
                                   double& minReducedCost)
{
  EdgeSet forbiddenEdges(originalGraph);
  const std::optional<double> lowerTimeBound;
  const std::optional<double> upperTimeBound;

  std::optional<double> minCost;

  auto paths = sparsePricer->findPaths(ReducedCosts(dualValues, graph.travelTimes()),
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

void SparseStabilizingPricer::checkUpperBound()
{
  const double upperBound = SCIPgetLPObjval(scip);

  // check that upper bound coincides with the one obtained from the dual variables
  double expectedUpperBound = 0.;

  const auto& coveringConstraints = getCoveringConstraints();

  for(const Vertex& vertex : originalGraph.getVertices())
  {
    expectedUpperBound += SCIPgetDualsolLinear(scip, coveringConstraints(vertex));
  }

  const auto& combinedVariables = program.getCombinedVariables();

  for(const Edge& edge : originalGraph.getEdges())
  {
    const double reducedCost = SCIPgetVarRedcost(scip, combinedVariables(edge));

    assert(reducedCost != SCIP_INVALID);

    expectedUpperBound += std::min(reducedCost, 0.);
  }

  const auto& variables = getVariables();

  for(const TimedEdge& timedEdge : graph.getEdges())
  {
    SCIP_VAR* var = variables(timedEdge);

    if(var == NULL)
    {
      continue;
    }

    const double reducedCost = SCIPgetVarRedcost(scip, var);

    assert(reducedCost != SCIP_INVALID);

    if(SCIPisFeasGT(scip, reducedCost, 0.))
    {
      const double value = SCIPgetVarSol(scip, var);

      assert(SCIPisFeasEQ(scip, value, 0.));
    }
    else if(SCIPisFeasLT(scip, reducedCost, 0.))
    {
      const double value = SCIPgetVarSol(scip, var);

      assert(SCIPisFeasEQ(scip, value, 1.));
    }

    expectedUpperBound += std::min(reducedCost, 0.);
  }

  assert(SCIPisFeasEQ(scip, expectedUpperBound, upperBound));
}

void SparseStabilizingPricer::checkDualFeasibility()
{

  const auto& linkingConstraints = getLinkingConstraints();

  const auto& coveringConstraints = getCoveringConstraints();

  const auto& combinedVariables = program.getCombinedVariables();

  for(const Edge& edge : originalGraph.getEdges())
  {
    const Vertex source = edge.getSource();

    const double linkingDual = SCIPgetDualsolLinear(scip, linkingConstraints(edge));

    const double coveringDual = SCIPgetDualsolLinear(scip, coveringConstraints(source));

    double reducedCost = SCIPgetVarRedcost(scip, combinedVariables(edge));

    assert(reducedCost != SCIP_INVALID);

    reducedCost = std::min(reducedCost, 0.);

    assert(SCIPisFeasGE(scip, linkingDual - reducedCost - coveringDual, 0.));
  }
}

SparseStabilizingPricer::DualValues
SparseStabilizingPricer::getCurrentDualValues()
{
  EdgeMap<double> currentDualValues = program.getDualValues(DualCostType::SIMPLE);

  // add reduced costs of variables
  {
    const auto& variables = getVariables();

    for(const TimedEdge& timedEdge : graph.getEdges())
    {
      SCIP_VAR* var = variables(timedEdge);

      if(var == NULL)
      {
        continue;
      }

      double reducedCost = SCIPgetVarRedcost(scip, var);

      assert(reducedCost != SCIP_INVALID);

      reducedCost = std::min(reducedCost, 0.);

      currentDualValues(timedEdge) += reducedCost;
    }
  }

  const double upperBound = SCIPgetLPObjval(scip);

  return DualValues(currentDualValues, 0., upperBound);
}

SparsePricingResult
SparseStabilizingPricer::performPricing(DualCostType costType)
{
  if(costType == DualCostType::FARKAS)
  {
    return sparsePricer->performPricing(costType);
  }

  if(program.hasFixedEdges())
  {
    return sparsePricer->performPricing(costType);
  }

  if(program.hasSeparator() && program.getSeparator().hasCuts())
  {
    return sparsePricer->performPricing(costType);
  }

  const DualValues currentDualValues = getCurrentDualValues();

  bool misPriced = false;
  const double upperBound = SCIPgetLPObjval(scip);

  Log(info) << "Performing stabilized reduced cost pricing";

  assert(centeredDualValues.getLagrangianBound());

  auto centeredBound = [&]() -> double
    {
      return *centeredDualValues.getLagrangianBound();
    };

  if(debuggingEnabled())
  {
    checkUpperBound();
    checkDualFeasibility();
  }

  do
  {
    bool movedCenter = false;

    // This can happen for acyclic paths
    if(upperBound < centeredBound())
    {
      Log(info) << "Current solution is below relaxation, falling back to standard pricing";

      return sparsePricer->performPricing(costType);
    }

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

        return SparsePricingResult(currentPaths, centeredBound());
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
        if(cmp::neg(path.cost(ReducedCosts(currentDualValues, graph.travelTimes()))))
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

    if(cutoffThreshold &&
       upperBound <= centeredBound() + *cutoffThreshold)
    {
      Log(info) << "Achieved optimum solution value";
      break;
    }

    if(misPriced)
    {
      assert(movedCenter);

      Log(info) << "Misprice. Continuing";
    }
    else
    {
      Log(info) << "Resolving master problem with "
                << currentPaths.size()
                << " new paths";

      return SparsePricingResult(currentPaths, centeredBound());
    }
  }
  while(misPriced);

  return SparsePricingResult::empty();
}
