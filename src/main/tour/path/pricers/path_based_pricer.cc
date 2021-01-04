#include "path_based_pricer.hh"

#include <sstream>

#include "log.hh"

#include "scip_utils.hh"

#include "tour/path/path_based_program.hh"

PathBasedPricer::PathBasedPricer(PathBasedProgram& program,
                                 const std::string& name,
                                 idx numPaths)
  : scip::ObjPricer(program.getSCIP(),
                    name.c_str(),
                    "Finds paths with negative reduced cost.",
                    0,
                    TRUE),
    program(program),
    maxNumPaths(numPaths),
    name(name),
    originalGraph(program.getOriginalGraph()),
    graph(program.getGraph()),
    scip(program.getSCIP()),
    linkingConstraints(program.getLinkingConstraints()),
    timeHorizon(program.getTimeHorizon()),
    originalSource(program.getSource()),
    distances(program.getDistances()),
    combinedVariables(graph, program.getCombinedVariables())
{
}

EdgeMap<double> PathBasedPricer::getDualValues(DualCostType costType) const
{
  EdgeMap<double> dualValues(graph, 0);

  if(costType == DualCostType::FARKAS)
  {
    for(const Edge& edge : originalGraph.getEdges())
    {
      const double dualValue = SCIPgetDualfarkasLinear(scip, linkingConstraints(edge));

      if(dualValue != 0.)
      {
        for(const TimedEdge& timedEdge : graph.getTimedEdges(edge))
        {
          dualValues(timedEdge) += dualValue;
        }
      }
    }
  }
  else
  {
    assert(costType == DualCostType::SIMPLE);

    for(const Edge& edge : originalGraph.getEdges())
    {
      const double dualValue = SCIPgetDualsolLinear(scip, linkingConstraints(edge));

      if(dualValue != 0.)
      {
        for(const TimedEdge& timedEdge : graph.getTimedEdges(edge))
        {
          dualValues(timedEdge) += dualValue;
        }
      }
    }
  }

  return dualValues;
}

const EdgeMap<SCIP_VAR*>&
PathBasedPricer::getCombinedVariables() const
{
  return combinedVariables;
}

void PathBasedPricer::addTimedPaths(const std::vector<TimedPath>& timedPaths)
{
  for(const TimedPath& timedPath : timedPaths)
  {
    Path path = timedPath.underlyingPath(graph);

    PathVariable pathVariable = addTimedPath(timedPath, true);

    if(path.isTour(originalGraph))
    {
      addSolution(pathVariable, path);
    }
  }
}

PathBasedPricingResult PathBasedPricer::performPricing(DualCostType costType)
{
  if(costType == DualCostType::SIMPLE)
  {
    return performPricing(ReducedCosts(graph, getDualValues(DualCostType::SIMPLE)));
  }
  else
  {
    assert(costType == DualCostType::FARKAS);

    return performPricing(FarkasCosts(graph, getDualValues(DualCostType::FARKAS)));
  }
}

PathBasedPricingResult PathBasedPricer::performPricing(const EdgeFunc<double>& reducedCosts)
{
  std::optional<double> lowerTimeBound;
  std::optional<double> upperTimeBound;

  const bool solveRelaxation = program.getSettings().solveRelaxation;

  if(!solveRelaxation)
  {
    lowerTimeBound = program.lowerBound();
    upperTimeBound = program.upperBound();
  }

  const EdgeSet forbiddenEdges = program.getForbiddenEdges();

  std::optional<double> minReducedCosts;

  std::vector<TimedPath> timedPaths = findPaths(reducedCosts,
                                                forbiddenEdges,
                                                lowerTimeBound,
                                                upperTimeBound,
                                                minReducedCosts);

  if(timedPaths.empty())
  {
    Log(info) << "Did not find any paths";

    return PathBasedPricingResult::empty();
  }
  else
  {
    Log(info) << "Found " << timedPaths.size() << " paths";

    if(minReducedCosts)
    {
      const double upperBound = SCIPgetLPObjval(scip);

      const double lowerBound = upperBound + *minReducedCosts;

      return PathBasedPricingResult(timedPaths, lowerBound);
    }
    else
    {
      return PathBasedPricingResult(timedPaths);
    }
  }
}

void PathBasedPricer::addResult(const PathBasedPricingResult& result,
                                double* lowerBound)
{
  addTimedPaths(result.getPaths());

  if(lowerBound && result.getLowerBound())
  {
    *lowerBound = *(result.getLowerBound());
  }
}

SCIP_DECL_PRICERINIT(PathBasedPricer::scip_init)
{

  for(const Edge& edge: originalGraph.getEdges())
  {
    SCIP_CONS* cons;

    SCIP_CALL(SCIPgetTransformedCons(scip,
                                     linkingConstraints(edge),
                                     &cons));

    linkingConstraints(edge) = cons;
  }

  for(const Edge& edge : originalGraph.getEdges())
  {
    SCIP_VAR* var;

    SCIP_CALL(SCIPgetTransformedVar(scip,
                                    combinedVariables(edge),
                                    &var));

    combinedVariables(edge) = var;
  }


  return SCIP_OKAY;
}

SCIP_DECL_PRICERREDCOST(PathBasedPricer::scip_redcost)
{
  *result = SCIP_SUCCESS;

  Log(info) << "Performing reduced cost pricing";

  auto pricingResult = performPricing(DualCostType::SIMPLE);

  addResult(pricingResult, lowerbound);

  return SCIP_OKAY;
}

SCIP_DECL_PRICERFARKAS(PathBasedPricer::scip_farkas)
{
  *result = SCIP_SUCCESS;

  Log(info) << "Performing Farkas pricing";

  auto pricingResult = performPricing(DualCostType::FARKAS);

  addResult(pricingResult, nullptr);

  return SCIP_OKAY;
}

SCIP_DECL_PRICEREXIT(PathBasedPricer::scip_exit)
{
  releaseVariables();

  return SCIP_OKAY;
}


PathVariable PathBasedPricer::addTimedPath(const TimedPath& timedPath,
                                           bool pricing)
{
  if(debuggingEnabled())
  {
    // Ensure that we don't add duplicate variables
    for(const PathVariable& pathVariable : getPathVariables())
    {
      const TimedPath& variablePath = pathVariable.getTimedPath();
      assert(variablePath != timedPath);
    }
  }

  const Path underlyingPath = timedPath.underlyingPath(graph);

  assert(underlyingPath.connects(originalSource, originalSource));

  num cost = underlyingPath.cost(distances);

  SCIP_VAR* var;

  std::ostringstream namebuf;

  namebuf << "path_" << pathVariables.size();

  const bool solveRelaxation = program.getSettings().solveRelaxation;

  // Note: We add the variables without upper bound to avoid
  // introducing additional dual variables. The
  // binary edge variables should be sufficient to ensure
  // that the selected path variable is binary as well...
  SCIP_CALL_EXC(SCIPcreateVar(scip,
                              &var,
                              namebuf.str().c_str(),
                              0.,
                              SCIPinfinity(scip),
                              cost,
                              solveRelaxation ?
                              SCIP_VARTYPE_CONTINUOUS :
                              SCIP_VARTYPE_IMPLINT,
                              TRUE,
                              TRUE,
                              NULL, NULL, NULL, NULL, NULL));

  SCIP_CALL_EXC(SCIPchgVarBranchPriority(scip, var, 0));

  if(pricing)
  {
    SCIP_CALL_EXC(SCIPaddPricedVar(scip, var, 1.));
  }
  else
  {
    SCIP_CALL_EXC(SCIPaddVar(scip, var));
  }

  EdgeMap<idx> counts(originalGraph, 0);

  for(const Edge& edge : underlyingPath.getEdges())
  {
    ++counts(edge);
  }

  for(const Edge& edge : originalGraph.getEdges())
  {
    const idx edgeCount = counts(edge);

    if(edgeCount > 0)
    {
      SCIP_CALL_EXC(SCIPaddCoefLinear(scip, linkingConstraints(edge), var, edgeCount));
    }
  }

  PathVariable pathVariable{timedPath, var};

  pathVariables.push_back(pathVariable);

  return pathVariable;
}

void PathBasedPricer::addTour(const Tour& tour, bool pricing)
{
  Log(info) << "Adding tour";

  Path path = tour.asPath();

  TimedPath timedPath = graph.expandPath(path);

  PathVariable pathVariable = addTimedPath(timedPath, pricing);

  addSolution(pathVariable, path);
}

void PathBasedPricer::addSolution(const PathVariable& pathVariable,
                                  const Path& path) const
{
  SCIP_SOL* solution;
  const EdgeMap<SCIP_VAR*>& edgeVariables = program.getCombinedVariables();

  SCIP_CALL_EXC(SCIPcreateSol(scip, &solution, nullptr));

  SCIP_CALL_EXC(SCIPsetSolVal(scip, solution, pathVariable.getVariable(), 1));

  for(const Edge& edge : path.getEdges())
  {
    SCIP_CALL_EXC(SCIPsetSolVal(scip, solution, edgeVariables(edge), 1));
  }

  SCIP_Bool stored;

  SCIP_CALL_EXC(SCIPaddSolFree(scip, &solution, &stored));
}

void PathBasedPricer::printPaths(SCIP_SOL* sol) const
{
  double totalValue = 0;
  idx size = 0;

  for(const PathVariable& pathVariable : pathVariables)
  {
    SCIP_VAR* var = pathVariable.getVariable();

    double value = SCIPgetSolVal(scip, sol, var);

    if(value == 0)
    {
      continue;
    }

    totalValue += value;
    size++;

    {
      std::ostringstream msgbuf;

      msgbuf << "Value: " << value << " for ";

      for(const TimedVertex& timedVertex : pathVariable.getTimedPath().getVertices())
      {
        Vertex vertex = graph.underlyingVertex(timedVertex);

        msgbuf << vertex.getIndex() << ", ";
      }

      msgbuf << "[" << SCIPvarGetObj(var) << "]";

      Log(info) << msgbuf.str();
    }
  }

  Log(info) << "Total number of paths: "
            << size
            << ", total flow: "
            << totalValue;

}

void PathBasedPricer::releaseVariables()
{
  for(PathVariable& pathVariable : pathVariables)
  {
    SCIP_VAR* var = pathVariable.getVariable();

    SCIP_CALL_EXC(SCIPreleaseVar(scip, &var));

    pathVariable.setVariable(nullptr);
  }

  Log(debug) << "Released variables";
}

PathBasedPricer::~PathBasedPricer() {}
