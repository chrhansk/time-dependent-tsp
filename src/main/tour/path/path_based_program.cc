#include "path_based_program.hh"

#include <unordered_map>
#include <unordered_set>
#include <sstream>

#include "log.hh"

#include "router/label.hh"
#include "router/label_heap.hh"

#include "timed/time_expanded_graph.hh"
#include "tour/timed/expand_tour.hh"

#include "tour/path/pricers/pricers.hh"

#include "scip_utils.hh"

PathBasedProgram::PathBasedProgram(const Tour& initialTour,
                                   TimedDistanceFunc& distances,
                                   num lowerBound,
                                   bool withPricer,
                                   const Program::Settings& settings)
  : Program("path_based_timed_tour", settings),
    distances(distances),
    initialTour(initialTour),
    source(initialTour.getSource()),
    originalGraph(initialTour.getGraph()),
    graph(createTimeExpandedGraph(initialTour, distances, lowerBound)),
    timeHorizon(initialTour.cost(distances)),
    pricer(nullptr),
    combinedVariables(graph, nullptr),
    linkingConstraints(graph, nullptr),
    coveringConstraints(graph, nullptr)
{
  createVariables();
  createConstraints();

  if(withPricer)
  {
    pricer = new PathBasedSimplePricer(*this);
  }
}

void PathBasedProgram::setPricer(PathBasedPricer* p)
{
  assert(!pricer);

  pricer = p;
}

void PathBasedProgram::createVariables()
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
                                     0.,
                                     1.,
                                     0.,
                                     solveRelaxation ?
                                     SCIP_VARTYPE_CONTINUOUS :
                                     SCIP_VARTYPE_BINARY));

    SCIP_CALL_EXC(SCIPchgVarBranchPriority(scip, var, 1));

    SCIP_CALL_EXC(SCIPaddVar(scip, var));

    combinedVariables(edge) = var;
  }
}

void PathBasedProgram::createConstraints()
{
  Log(info) << "Creating constraints";

  for(const Vertex& vertex : originalGraph.getVertices())
  {
    SCIP_CONS* cons;

    std::ostringstream namebuf;

    namebuf << "covering_" << vertex.getIndex();

    SCIP_CALL_EXC(SCIPcreateConsLinear(scip,
                                       &cons,
                                       namebuf.str().c_str(),
                                       0,
                                       NULL,
                                       NULL,
                                       1.,      // lhs
                                       1.,      // rhs
                                       TRUE,    // initial
                                       TRUE,    // separate
                                       TRUE,    // enforce
                                       TRUE,    // check
                                       TRUE,    // propagate
                                       FALSE,   // local
                                       TRUE,    // modifiable
                                       FALSE,   // dynamic
                                       FALSE,   // removable
                                       FALSE)); // sticking at node

    for(const Edge& edge : originalGraph.getOutgoing(vertex))
    {
      SCIP_CALL_EXC(SCIPaddCoefLinear(scip,
                                      cons,
                                      combinedVariables(edge),
                                      1.));
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
                                       0.,                 // lhs
                                       0.,                 // rhs
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
                                    -1.));

    linkingConstraints(edge) = cons;
    SCIP_CALL_EXC(SCIPaddCons(scip, cons));
  }
}

const std::vector<PathVariable>&
PathBasedProgram::getPathVariables() const
{
  return pricer->getPathVariables();
}

SolutionResult PathBasedProgram::solveRelaxation(int timeLimit)
{
  assert(pricer);

  if(timeLimit != -1)
  {
    SCIP_CALL_EXC(SCIPsetRealParam(scip, "limits/time", timeLimit));
  }

  {
    SCIP_CALL_EXC(SCIPincludeObjPricer(scip, pricer, true));

    /* activate pricer */

    SCIP_PRICER* SCIPPricer = SCIPfindPricer(scip, pricer->getName().c_str());

    assert(SCIPPricer);

    SCIP_CALL_EXC(SCIPactivatePricer(scip, SCIPPricer));

    // for some reason, SCIP adds cutting planes otherwise?!
    SCIP_CALL_EXC(SCIPsetIntParam(scip, "separating/maxruns", 0));

    SCIP_CALL_EXC(SCIPsetIntParam(scip, "separating/maxrounds", 0));

    SCIP_CALL_EXC(SCIPsetIntParam(scip, "separating/maxcuts", 0));

  }

  SCIP_CALL_EXC(SCIPsolve(scip));


  SCIP_SOL* solution = SCIPgetBestSol(scip);

  if(!solution)
  {
    throw std::invalid_argument("No solution found");
  }

  return SolutionResult(SCIPgetSolOrigObj(scip, solution),
                        getStats());
}

SolutionResult PathBasedProgram::solve(int timeLimit)
{
  assert(pricer);

  if(timeLimit != -1)
  {
    SCIP_CALL_EXC(SCIPsetRealParam(scip, "limits/time", timeLimit));
  }

  {
    SCIP_CALL_EXC(SCIPincludeObjPricer(scip, pricer, true));

    /* activate pricer */

    SCIP_PRICER* SCIPPricer = SCIPfindPricer(scip, pricer->getName().c_str());

    assert(SCIPPricer);

    SCIP_CALL_EXC(SCIPactivatePricer(scip, SCIPPricer));

    // for some reason, SCIP adds cutting planes otherwise?!
    SCIP_CALL_EXC(SCIPsetIntParam(scip, "separating/maxruns", 0));

    SCIP_CALL_EXC(SCIPsetIntParam(scip, "separating/maxrounds", 0));

    SCIP_CALL_EXC(SCIPsetIntParam(scip, "separating/maxcuts", 0));

  }

  pricer->addTour(initialTour, false);

  SCIP_CALL_EXC(SCIPsolve(scip));

  SCIP_SOL* solution = SCIPgetBestSol(scip);

  if(!solution)
  {
    throw std::invalid_argument("No solution found");
  }

  pricer->printPaths(solution);

  for(const PathVariable& pathVariable : pricer->getPathVariables())
  {
    double value = SCIPgetSolVal(scip, solution, pathVariable.getVariable());

    if(value <= 0.5)
    {
      continue;
    }

    assert(SCIPisFeasIntegral(scip, value));

    std::vector<Vertex> vertices;

    for(const TimedEdge& timedEdge : pathVariable.getTimedPath().getEdges())
    {
      Edge edge = graph.underlyingEdge(timedEdge);
      vertices.push_back(edge.getSource());
    }

    assert(vertices.size() == originalGraph.getVertices().size());

    Tour tour(graph, vertices);

    assert(tour.connects(originalGraph.getVertices().collect()));

    return SolutionResult(tour, getStats());
  }

  throw std::logic_error("Could not find a nonzero path variable");
}

EdgeMap<double> PathBasedProgram::getFlow() const
{
  EdgeMap<double> values(graph, 0);

  for(const PathVariable& variable : pricer->getPathVariables())
  {
    double value = SCIPgetVarSol(scip, variable.getVariable());

    for(const TimedEdge& timedEdge : variable.getTimedPath().getEdges())
    {
      Edge edge = graph.underlyingEdge(timedEdge);
      values(edge) += value;
    }
  }

  return values;
}

bool PathBasedProgram::hasFixedEdges() const
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

EdgeSet PathBasedProgram::getForbiddenEdges() const
{
  EdgeSet forbiddenEdges(graph);

  for(const Edge& edge : originalGraph.getEdges())
  {
    if(SCIPvarGetUbLocal(combinedVariables(edge)) <= 0.5)
    {
      forbiddenEdges.insert(edge);
    }
  }

  return forbiddenEdges;
}

EdgeSet PathBasedProgram::getRequiredEdges() const
{
  EdgeSet requiredEdges(graph);

  for(const Edge& edge : originalGraph.getEdges())
  {
    if(SCIPvarGetLbLocal(combinedVariables(edge)) >= 0.5)
    {
      requiredEdges.insert(edge);
    }
  }

  return requiredEdges;
}

PathBasedProgram::~PathBasedProgram()
{
  for(const Vertex& vertex : originalGraph.getVertices())
  {
    SCIP_CONS* cons = coveringConstraints(vertex);
    if(cons)
    {
      SCIP_CALL_ASSERT(SCIPreleaseCons(scip, &cons));
    }
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
