#include "sparse_program.hh"

#include <sstream>

#include <scip/scipdefplugins.h>

#include "scip_utils.hh"

#include "sparse_objective_propagator.hh"

#include "tour/timed/expand_tour.hh"

#include "tour/sparse/separators/sparse_separation_manager.hh"

#include "tour/sparse/pricers/sparse_path_router_pricer.hh"

#include "tour/sparse/pricers/sparse_pricing_manager.hh"

SparseProgram::SparseProgram(const Tour& initialTour,
                             TimedDistanceFunc& distances,
                             num lowerBound,
                             bool withPricer,
                             const Program::Settings& settings)
  : Program("sparse_timed_tour", settings),
    distances(distances),
    initialTour(initialTour),
    source(initialTour.getSource()),
    graph(createTimeExpandedGraph(initialTour, distances, lowerBound)),
    originalGraph(graph.underlyingGraph()),
    combinedVariables(originalGraph, nullptr),
    linkingConstraints(originalGraph, nullptr),
    coveringConstraints(originalGraph, nullptr),
    separator(nullptr),
    pricer(nullptr)
{
  TimedDistanceEvaluator evaluator(distances);

  timeHorizon = evaluator(initialTour);

  pricer = new SparsePricingManager(*this);

  if(withPricer)
  {
    pricer->setPricer(std::make_unique<SparseSimplePathPricer>(*this));
  }

}

void SparseProgram::createVariables()
{
  const bool solveRelaxation = getSettings().solveRelaxation;

  for(const Edge& edge : originalGraph.getEdges())
  {
    SCIP_VAR* var;

    std::ostringstream namebuf;

    namebuf << "x_" << edge.getSource().getIndex()
            << "_" << edge.getTarget().getIndex();

    SCIP_CALL_EXC(SCIPcreateVarBasic(scip,
                                     &var,
                                     namebuf.str().c_str(),
                                     0.0,
                                     1.0,
                                     0.0,
                                     solveRelaxation ?
                                     SCIP_VARTYPE_CONTINUOUS :
                                     SCIP_VARTYPE_BINARY));

    SCIP_CALL_EXC(SCIPchgVarBranchPriority(scip, var, 1));

    SCIP_CALL_EXC(SCIPaddVar(scip, var));

    combinedVariables(edge) = var;
  }
}

void SparseProgram::createConstraints()
{
  Log(info) << "Creating constraints";

  for(const Vertex& vertex : originalGraph.getVertices())
  {
    SCIP_CONS* cons;

    std::ostringstream namebuf;

    namebuf << "covering_" << vertex.getIndex();

    // the constraint is not really modifiable,
    // but this way, it seems to at least survive the
    // preprocessing...

    SCIP_CALL_EXC(SCIPcreateConsLinear(scip,
                                       &cons,
                                       namebuf.str().c_str(),
                                       0,
                                       NULL,
                                       NULL,
                                       1.0,                // lhs
                                       1.0,                // rhs
                                       TRUE,               // initial
                                       TRUE,               // separate
                                       TRUE,               // enforce
                                       TRUE,               // check
                                       TRUE,               // propagate
                                       FALSE,              // local
                                       TRUE,               // modifiable
                                       FALSE,              // dynamic
                                       FALSE,              // removable
                                       FALSE));            // sticking at node

    for(const Edge& edge : originalGraph.getOutgoing(vertex))
    {
      SCIP_CALL_EXC(SCIPaddCoefLinear(scip,
                                      cons,
                                      combinedVariables(edge),
                                      1.0));
    }

    coveringConstraints(vertex) = cons;
    SCIP_CALL_EXC(SCIPaddCons(scip, cons));
  }

  for(const Edge& edge : originalGraph.getEdges())
  {
    SCIP_CONS* cons;

    std::ostringstream namebuf;

    namebuf << "linking_" << edge.getSource().getIndex()
            << "_" << edge.getTarget().getIndex();

    SCIP_CALL_EXC(SCIPcreateConsLinear(scip,
                                       &cons,
                                       namebuf.str().c_str(),
                                       0,
                                       NULL,
                                       NULL,
                                       0.0,                // lhs
                                       0.0,                // rhs
                                       TRUE,               // initial
                                       TRUE,               // separate
                                       TRUE,               // enforce
                                       TRUE,               // check
                                       TRUE,               // propagate
                                       FALSE,              // local
                                       TRUE,               // modifiable
                                       FALSE,              // dynamic
                                       FALSE,              // removable
                                       FALSE));            // sticking at node

    SCIP_CALL_EXC(SCIPaddCoefLinear(scip,
                                    cons,
                                    combinedVariables(edge),
                                    -1.0));

    linkingConstraints(edge) = cons;
    SCIP_CALL_EXC(SCIPaddCons(scip, cons));
  }

}

void SparseProgram::addHeuristic(scip::ObjHeur* heur)
{
  assert(pricer);

  SCIP_CALL_EXC(SCIPincludeObjHeur(scip, heur, TRUE));
}

void SparseProgram::addPropagator(scip::ObjProp* propagator)
{
  assert(pricer);

  SCIP_CALL_EXC(SCIPincludeObjProp(scip, propagator, TRUE));
}

void SparseProgram::addSeparator(SparseSeparationManager* sepa)
{
  assert(pricer);
  assert(!separator);

  separator = sepa;

  SCIP_CALL_EXC(SCIPincludeObjSepa(scip, separator, TRUE));
}

SolutionResult SparseProgram::solveRelaxation(int timeLimit)
{
  assert(getSettings().solveRelaxation);

  if(timeLimit != -1)
  {
    SCIPsetRealParam(scip, "limits/time", timeLimit);
  }

  Log(info) << "Solving time horizon = " << timeHorizon;

  createVariables();
  createConstraints();

  SCIP_CALL_EXC(SCIPincludeObjPricer(scip, pricer, TRUE));

  SCIP_CALL_EXC(SCIPactivatePricer(scip, SCIPfindPricer(scip, pricer->getName().c_str())));

  SCIP_CALL_EXC(SCIPsolve(scip));


  SCIP_SOL* solution = SCIPgetBestSol(scip);

  if(!solution)
  {
    throw std::invalid_argument("No solution found");
  }

  return SolutionResult(SCIPgetSolOrigObj(scip, solution),
                        getStats());
}

SolutionResult SparseProgram::solve(int timeLimit)
{
  assert(!getSettings().solveRelaxation);

  if(timeLimit != -1)
  {
    SCIPsetRealParam(scip, "limits/time", timeLimit);
  }

  Log(info) << "Solving time horizon = " << timeHorizon;

  createVariables();
  createConstraints();

  SCIP_CALL_EXC(SCIPincludeObjPricer(scip, pricer, TRUE));

  SCIP_CALL_EXC(SCIPactivatePricer(scip, SCIPfindPricer(scip, pricer->getName().c_str())));

  SCIP_CALL_EXC(SCIPsolve(scip));

  SCIP_SOL* solution = SCIPgetBestSol(scip);

  if(!solution)
  {
    throw std::invalid_argument("No solution found");
  }

  return SolutionResult(createTour(solution), getStats());
}

void SparseProgram::setPricer(SparsePricer* newPricer)
{
  assert(pricer);

  pricer->setPricer(std::unique_ptr<SparsePricer>(newPricer));
}

Tour SparseProgram::createTour(SCIP_SOL* solution) const
{
  assert(solution);

  const EdgeMap<SCIP_VAR*>& variables = getPricingManager().getVariables();

  TimedVertex currentVertex = graph.getVertex(source, 0);

  std::vector<Vertex> vertices;

  while(true)
  {
    bool found = false;

    vertices.push_back(graph.underlyingVertex(currentVertex));

    for(TimedEdge timedEdge : graph.getOutgoing(currentVertex))
    {
      SCIP_VAR* var = variables(timedEdge);

      if(!var)
      {
        continue;
      }

      double value = SCIPgetSolVal(scip, solution, var);

      if(value > 0.5)
      {

        Log(debug) << "Walking from "
                   << graph.underlyingVertex(timedEdge.getSource()).getIndex()
                   << " to "
                   << graph.underlyingVertex(timedEdge.getTarget()).getIndex()
                   << ", starting at "
                   << timedEdge.getSource().getTime()
                   << ", arriving at "
                   << timedEdge.getTarget().getTime()
                   << " (value: "
                   << value
                   << ")";

        currentVertex = timedEdge.getTarget();
        found = true;

        break;
      }
    }

    if(!found)
    {
      break;
    }
  }

  assert(graph.underlyingVertex(currentVertex) == source);

  idx numVertices = originalGraph.getVertices().size();

  assert(vertices.size() == numVertices + 1);

  assert(vertices.front() == source);
  assert(vertices.back() == source);

  vertices.resize(numVertices);

  return Tour(originalGraph, vertices);
}

void SparseProgram::addedEdge(const TimedEdge& timedEdge)
{
  if(separator)
  {
    separator->addedEdge(timedEdge);
  }
}

SparseProgram::~SparseProgram()
{
  for(const Vertex& vertex : originalGraph.getVertices())
  {
    SCIP_CONS* cons = coveringConstraints(vertex);

    if(!cons)
    {
      continue;
    }

    SCIP_CALL_ASSERT(SCIPreleaseCons(scip, &cons));
  }

  coveringConstraints.reset(nullptr);

  for(const Edge& edge : originalGraph.getEdges())
  {
    SCIP_CONS* cons = linkingConstraints(edge);

    if(!cons)
    {
      continue;
    }

    SCIP_CALL_ASSERT(SCIPreleaseCons(scip, &cons));
  }

  linkingConstraints.reset(nullptr);

  for(const Edge& edge : originalGraph.getEdges())
  {
    SCIP_VAR* var = combinedVariables(edge);

    if(!var)
    {
      continue;
    }

    SCIP_CALL_ASSERT(SCIPreleaseVar(scip, &var));
  }

  combinedVariables.reset(nullptr);

  SCIP_CALL_ASSERT(SCIPfree(&scip));
}

bool SparseProgram::hasFixedEdges() const
{
  for(const Edge& edge : originalGraph.getEdges())
  {
    if(SCIPvarGetUbLocal(combinedVariables(edge)) <= 0.5)
    {
      return true;
    }
    else if(SCIPvarGetLbLocal(combinedVariables(edge)) >= 0.5)
    {
      return true;
    }
  }

  return false;
}

EdgeSet SparseProgram::getForbiddenEdges() const
{
  EdgeSet forbiddenEdges(originalGraph);

  for(const Edge& edge : originalGraph.getEdges())
  {
    if(SCIPvarGetUbLocal(combinedVariables(edge)) <= 0.5)
    {
      forbiddenEdges.insert(edge);
    }
  }

  return forbiddenEdges;
}

EdgeMap<double> SparseProgram::getDualValues(DualCostType costType) const
{
  EdgeMap<double> dualValues(graph, 0);

  // linking constraints
  {
    const EdgeMap<SCIP_CONS*>& linkingConstraints = getPricingManager().getLinkingConstraints();

    for(const Edge& originalEdge : originalGraph.getEdges())
    {
      double linkingCost = 0;

      if(costType == DualCostType::FARKAS)
      {
        linkingCost = SCIPgetDualfarkasLinear(scip, linkingConstraints(originalEdge));
      }
      else
      {
        assert(costType == DualCostType::SIMPLE);

        linkingCost = SCIPgetDualsolLinear(scip, linkingConstraints(originalEdge));
      }

      for(const TimedEdge& timedEdge : graph.getTimedEdges(originalEdge))
      {
        dualValues(timedEdge) += linkingCost;
      }
    }
  }

  // flow constraints (stored in pricer)
  {
    const VertexMap<SCIP_CONS*>& flowConstraints = getPricingManager().getFlowConstraints();

    for(const TimedVertex& timedVertex : graph.getVertices())
    {
      SCIP_CONS* flowConstraint = flowConstraints(timedVertex);

      if(!flowConstraint)
      {
        continue;
      }

      double flowCost = 0;

      if(costType == DualCostType::FARKAS)
      {
        flowCost = SCIPgetDualfarkasLinear(scip, flowConstraint);
      }
      else
      {
        assert(costType == DualCostType::SIMPLE);

        flowCost = SCIPgetDualsolLinear(scip, flowConstraint);
      }

      if(flowCost == 0.)
      {
        continue;
      }

      for(const TimedEdge& outgoing : graph.getOutgoing(timedVertex))
      {
        dualValues(outgoing) += flowCost;
      }

      for(const TimedEdge& incoming : graph.getIncoming(timedVertex))
      {
        dualValues(incoming) += -flowCost;
      }
    }
  }

  // separated inequalities
  if(separator)
  {
    separator->addDualCosts(dualValues, costType);
  }

  return dualValues;
}

SparseSolutionValues SparseProgram::solutionValues() const
{
  return SparseSolutionValues(getSCIP(),
                              getPricingManager().getVariables().getValues());
}
