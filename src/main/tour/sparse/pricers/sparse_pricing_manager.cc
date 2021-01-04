#include "sparse_pricing_manager.hh"

#include <sstream>

#include "tour/sparse/sparse_program.hh"

#include "sparse_pricer.hh"
#include "sparse_pricing_result.hh"

#define NAME "sparse_pricer"

SparsePricingManager::SparsePricingManager(SparseProgram& program,
                                           std::unique_ptr<SparsePricer>&& sparsePricer)
  : scip::ObjPricer(program.getSCIP(),
                    NAME,
                    "Finds variables with negative reduced costs",
                    0,
                    TRUE),
  program(program),
  sparsePricer(std::move(sparsePricer)),
  scip(program.getSCIP()),
  graph(program.getGraph()),
  distances(program.getDistances()),
  timeHorizon(program.getTimeHorizon()),
  source(program.getSource()),
  coveringConstraints(program.getCoveringConstraints()),
  linkingConstraints(program.getLinkingConstraints()),
  flowConstraints(graph, nullptr),
  variables(graph, nullptr),
  initiated(false)
{
}

SparsePricingManager::SparsePricingManager(SparseProgram& program)
  : SparsePricingManager(program, std::unique_ptr<SparsePricer>())
{}

SCIP_DECL_PRICERINIT(SparsePricingManager::scip_init)
{
  const Graph& originalGraph = graph.underlyingGraph();

  linkingConstraints = program.getLinkingConstraints();
  coveringConstraints = program.getCoveringConstraints();

  for(const Edge& edge : originalGraph.getEdges())
  {
    SCIP_CONS* cons;

    SCIP_CALL_EXC(SCIPgetTransformedCons(scip,
                                         linkingConstraints(edge),
                                         &cons));

    linkingConstraints(edge) = cons;
  }

  for(const Vertex& vertex : originalGraph.getVertices())
  {
    SCIP_CONS* cons;

    SCIP_CALL_EXC(SCIPgetTransformedCons(scip,
                                         coveringConstraints(vertex),
                                         &cons));

    coveringConstraints(vertex) = cons;
  }

  if(debuggingEnabled())
  {
    for(const TimedVertex& timedVertex : graph.getVertices())
    {
      assert(flowConstraints(timedVertex) == nullptr);
    }
  }

  return SCIP_OKAY;
}

SCIP_DECL_PRICEREXIT(SparsePricingManager::scip_exit)
{
  for(const TimedVertex& timedVertex : graph.getVertices())
  {
    SCIP_CONS* cons = flowConstraints(timedVertex);

    if(!cons)
    {
      continue;
    }

    SCIP_CALL_EXC(SCIPreleaseCons(scip, &cons));
  }

  for(const TimedEdge& timedEdge : graph.getEdges())
  {
    SCIP_VAR* var = variables(timedEdge);
    if(!var)
    {
      continue;
    }
    SCIP_CALL_EXC(SCIPreleaseVar(scip, &var));
  }

  variables.reset(nullptr);

  flowConstraints.reset(nullptr);

  return SCIP_OKAY;
}

std::string SparsePricingManager::getName() const
{
  return NAME;
}

bool SparsePricingManager::addTour(const Tour& tour,
                                   SCIP_HEUR* heur)
{
  TimedVertex sourceVertex = graph.getVertex(source, 0);

  Path path = tour.asPath();

  TimedVertex currentVertex = sourceVertex;

  TimedPath timedPath;

  for(const Edge& edge : path.getEdges())
  {
    bool found = false;

    for(const TimedEdge& timedEdge : graph.getOutgoing(currentVertex))
    {
      if(graph.underlyingEdge(timedEdge) == edge)
      {
        currentVertex = timedEdge.getTarget();
        addEdge(timedEdge);
        timedPath.append(timedEdge);
        found = true;
        break;
      }
    }

    assert(found);
  }

  return addSolution(timedPath, heur);
}

bool SparsePricingManager::addSolution(const TimedPath& path,
                                       SCIP_HEUR* heur)
{
  SCIP_SOL* solution;
  SCIP_CALL_EXC(SCIPcreateSol(scip, &solution, heur));

  const EdgeMap<SCIP_VAR*>& combinedVariables = program.getCombinedVariables();

  for(const TimedEdge& timedEdge : path.getEdges())
  {
    SCIP_VAR* variable = variables(timedEdge);

    assert(variable);

    SCIP_CALL_EXC(SCIPsetSolVal(scip, solution, variable, 1));

    SCIP_VAR* combinedVariable = combinedVariables(graph.underlyingEdge(timedEdge));

    assert(combinedVariable);

    SCIP_CALL_EXC(SCIPsetSolVal(scip, solution, combinedVariable, 1));
  }

  SCIP_Bool stored;

  SCIP_CALL_EXC(SCIPaddSolFree(scip, &solution, &stored));

  Log(info) << "Solution was stored? " << std::boolalpha << !!stored;

  return stored;
}

void SparsePricingManager::addPath(const TimedPath& path)
{
  assert(!path.isEmpty());

  for(const TimedEdge& timedEdge : path.getEdges())
  {
    addEdge(timedEdge);
  }

  const Graph& originalGraph = graph.underlyingGraph();

  if(path.getEdges().size() == originalGraph.getVertices().size())
  {
    if(path.underlyingPath(graph).isTour(originalGraph))
    {
      addSolution(path);
    }
  }
}


void SparsePricingManager::addVertex(const TimedVertex& timedVertex)
{
  SCIP_CONS* cons = nullptr;

  const Vertex& vertex = graph.underlyingVertex(timedVertex);
  const idx& time = timedVertex.getTime();

  if(vertex == source)
  {
    return;
  }

  if(!flowConstraints(timedVertex))
  {
    std::ostringstream namebuf;

    namebuf << "flow_conservation_" << vertex.getIndex() << "#" << time;


    SCIP_CALL_EXC(SCIPcreateConsLinear(scip,
                                       &cons,
                                       namebuf.str().c_str(),
                                       0,
                                       NULL,
                                       NULL,
                                       0.0,     //lhs
                                       0.0,     //rhs
                                       TRUE,    // initial
                                       TRUE,    // separate
                                       FALSE,   // enforce
                                       TRUE,    // check
                                       TRUE,    // propagate
                                       FALSE,   // local
                                       TRUE,    // modifiable
                                       FALSE,   // dynamic
                                       FALSE,   // removable
                                       FALSE)); // sticking at node

    SCIP_CALL_EXC(SCIPaddCons(scip, cons));

    flowConstraints(timedVertex) = cons;
  }
}

SCIP_VAR* SparsePricingManager::addEdge(const TimedEdge& timedEdge)
{
  {
    SCIP_VAR* var = variables(timedEdge);
    if(var)
    {
      return var;
    }
  }

  addVertex(timedEdge.getSource());
  addVertex(timedEdge.getTarget());

  const Edge& edge = graph.underlyingEdge(timedEdge);

  const num cost = timedEdge.travelTime();

  assert(cost > 0);

  SCIP_VAR* var;

  std::ostringstream namebuf;

  const bool solveRelaxation = program.getSettings().solveRelaxation;

  namebuf << "x_" << edge.getSource().getIndex()
          << "_" << edge.getTarget().getIndex()
          << "#" << timedEdge.getSource().getTime();

  SCIP_CALL_EXC(SCIPcreateVar(scip,
                              &var,
                              namebuf.str().c_str(),
                              0.0,
                              1.0,
                              cost,
                              solveRelaxation ?
                              SCIP_VARTYPE_CONTINUOUS :
                              SCIP_VARTYPE_BINARY,
                              TRUE,   // initial
                              TRUE,   // removable
                              NULL, NULL, NULL, NULL, NULL));

  SCIP_CALL_EXC(SCIPchgVarBranchPriority(scip, var, 0));

  SCIP_CALL_EXC(SCIPaddVar(scip, var));

  if(edge.getTarget() != source)
  {
    SCIP_CALL_EXC(SCIPaddCoefLinear(scip, flowConstraints(timedEdge.getTarget()), var, -1.0));
  }

  if(edge.getSource() != source)
  {
    SCIP_CALL_EXC(SCIPaddCoefLinear(scip, flowConstraints(timedEdge.getSource()), var, 1.0));
  }

  SCIP_CALL_EXC(SCIPaddCoefLinear(scip, linkingConstraints(edge), var, 1.0));

  variables(timedEdge) = var;

  program.addedEdge(timedEdge);

  return var;
}



bool SparsePricingManager::contains(const TimedVertex& timedVertex) const
{
  return !!flowConstraints(timedVertex);
}

bool SparsePricingManager::contains(const TimedEdge& timedEdge) const
{
  return !!variables(timedEdge);
}

SCIP_DECL_PRICERFARKAS(SparsePricingManager::scip_farkas)
{
  if(!initiated)
  {
    initiated = true;

    addTour(program.getInitialTour());

    return SCIP_OKAY;
  }

  assert(sparsePricer);

  auto pricingReuslt = sparsePricer->performPricing(DualCostType::FARKAS);

  addResult(pricingReuslt, DualCostType::FARKAS, NULL);

  *result = SCIP_SUCCESS;

  return SCIP_OKAY;
}

SCIP_DECL_PRICERREDCOST(SparsePricingManager::scip_redcost)
{
  assert(sparsePricer);

  auto pricingResult = sparsePricer->performPricing(DualCostType::SIMPLE);

  addResult(pricingResult, DualCostType::SIMPLE, lowerbound);

  *result = SCIP_SUCCESS;

  return SCIP_OKAY;
}

void SparsePricingManager::addResult(const SparsePricingResult& result,
                                     DualCostType dualCostType,
                                     double* lowerBound)
{
  for(const TimedPath& path : result.getPaths())
  {
    addPath(path);
  }

  for(const TimedEdge& edge : result.getEdges())
  {
    addEdge(edge);
  }
  
  if(lowerBound && result.getLowerBound())
  {
    *lowerBound = *(result.getLowerBound());
  }
}
