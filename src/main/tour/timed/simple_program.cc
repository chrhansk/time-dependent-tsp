#include "simple_program.hh"

#include <sstream>

#include <scip/scipdefplugins.h>

#include "log.hh"

#include "expand_tour.hh"

const double cutoff = 1e-6;

SimpleProgram::SimpleProgram(const Tour& initialTour,
                             TimedDistanceFunc& distances,
                             const Program::Settings& settings)
  : Program("simple_timed_tour", settings),
    graph(createTimeExpandedGraph(initialTour, distances)),
    originalGraph(graph.underlyingGraph()),
    distances(distances),
    initialTour(initialTour),
    variables(graph, nullptr),
    source(initialTour.getSource())
{
  createVariables(scip);

  createFlowConstraints(scip);
  createCoveringConstraints(scip);
}

void SimpleProgram::createVariables(SCIP* scip)
{
  std::ostringstream namebuf;

  const bool solveRelaxation = getSettings().solveRelaxation;

  for(const TimedEdge& timedEdge : graph.getEdges())
  {
    const num cost = timedEdge.travelTime();
    SCIP_VAR* var;

    namebuf.str("");
    namebuf << "x_" << graph.underlyingVertex(timedEdge.getSource()).getIndex()
            << "_" << graph.underlyingVertex(timedEdge.getTarget()).getIndex()
            << "#" << timedEdge.getSource().getTime();

    SCIP_CALL_EXC(SCIPcreateVarBasic(scip,
                                     &var,
                                     namebuf.str().c_str(),
                                     0.,   // lb
                                     1.,   // ub
                                     cost, // obj
                                     solveRelaxation ?
                                     SCIP_VARTYPE_CONTINUOUS :
                                     SCIP_VARTYPE_BINARY));

    SCIP_CALL_EXC(SCIPaddVar(scip, var));

    variables(timedEdge) = var;
  }
}

void SimpleProgram::addInitialSolution(SCIP* scip,
                                       const Tour& tour)
{
  Log(info) << "Creating initial solution";

  SCIP_SOL* solution;
  SCIP_CALL_EXC(SCIPcreateSol(scip, &solution, NULL));

  const std::vector<Vertex>& tourVertices = tour.getVertices();

  num currentTime = 0;

  //for(auto it = tourVertices.begin(); it != tourVertices.end(); ++it)
  for(idx i = 0; i < tourVertices.size(); ++i)
  {
    TimedVertex currentVertex = graph.getVertex(tourVertices[i], currentTime);

    idx j = (i + 1) % tourVertices.size();

    TimedVertex nextVertex = currentVertex;
    bool found = false;

    for(const TimedEdge& edge : graph.getOutgoing(currentVertex))
    {
      if(graph.underlyingVertex(edge.getTarget()) == tourVertices[j])
      {
        SCIPsetSolVal(scip, solution, variables(edge), 1);

        found = true;
        nextVertex = edge.getTarget();
        break;
      }
    }

    if(!found)
    {
      break;
    }

    currentTime = nextVertex.getTime();
  }

  SCIP_Bool stored;

  SCIP_CALL_EXC(SCIPaddSolFree(scip, &solution, &stored));

  assert(stored);
}

void SimpleProgram::createFlowConstraints(SCIP* scip)
{
  std::ostringstream namebuf;

  Log(info) << "Creating flow constraints";

  for(const TimedVertex& timedVertex : graph.getVertices())
  {
    if(graph.underlyingVertex(timedVertex) == getSource())
    {
      continue;
    }

    SCIP_CONS* cons;
    namebuf.str("");
    namebuf << "flow_conservation_"
            << timedVertex.getIndex()
            << "#"
            << timedVertex.getTime();

    SCIP_CALL_EXC(SCIPcreateConsLinear(scip,
                                       &cons,
                                       namebuf.str().c_str(),
                                       0,
                                       NULL,
                                       NULL,
                                       0.0, //lhs
                                       0.0, //rhs
                                       TRUE, TRUE, TRUE, TRUE, TRUE,
                                       FALSE, FALSE, FALSE, FALSE, FALSE));


    for(const TimedEdge& timedEdge : graph.getOutgoing(timedVertex))
    {
      SCIP_CALL_EXC(SCIPaddCoefLinear(scip, cons, variables(timedEdge), 1.0));
    }

    for(const TimedEdge& timedEdge : graph.getIncoming(timedVertex))
    {
      SCIP_CALL_EXC(SCIPaddCoefLinear(scip, cons, variables(timedEdge), -1.0));
    }

    SCIP_CALL_EXC(SCIPaddCons(scip, cons));
    SCIP_CALL_EXC(SCIPreleaseCons(scip, &cons));
  }
}

SimpleProgram::~SimpleProgram()
{
  // release variables
  for(const TimedEdge& timedEdge : graph.getEdges())
  {
    SCIP_VAR* var = variables(timedEdge);
    if(!var)
    {
      continue;
    }
    SCIP_CALL_ASSERT(SCIPreleaseVar(scip, &var));
  }

  variables.reset(nullptr);

  SCIP_CALL_ASSERT(SCIPfree(&scip));
}

SolutionResult SimpleProgram::solveRelaxation(int timeLimit )
{
  assert(getSettings().solveRelaxation);

  if(timeLimit != -1)
  {
    SCIPsetRealParam(scip, "limits/time", timeLimit);
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

SolutionResult SimpleProgram::solve(int timeLimit)
{
  assert(!getSettings().solveRelaxation);

  if(timeLimit != -1)
  {
    SCIPsetRealParam(scip, "limits/time", timeLimit);
  }

  addInitialSolution(scip, initialTour);

  SCIP_CALL_EXC(SCIPsolve(scip));
  SCIP_SOL* solution = SCIPgetBestSol(scip);

  if(!solution)
  {
    throw std::invalid_argument("No solution found");
  }

  return SolutionResult(createTour(solution), getStats());
}

void SimpleProgram::addSeparator(scip::ObjSepa* separator)
{
  SCIP_CALL_EXC(SCIPincludeObjSepa(scip, separator, TRUE));
}

void SimpleProgram::addBranchingRule(scip::ObjBranchrule* rule)
{
  SCIP_CALL_EXC(SCIPincludeObjBranchrule(scip, rule, TRUE));
}

void SimpleProgram::addConstraintHandler(scip::ObjConshdlr* handler)
{
  SCIP_CALL_EXC(SCIPincludeObjConshdlr(scip, handler, TRUE));
}

Tour SimpleProgram::createTour(SCIP_SOL* solution)
{
  assert(solution);

  const std::vector<Vertex>& vertices = initialTour.getVertices();

  TimedVertex currentVertex = graph.getVertex(vertices.front(), 0);

  std::vector<Vertex> tourVertices;
  tourVertices.reserve(vertices.size());

  while(true)
  {
    bool found = false;

    tourVertices.push_back(graph.underlyingVertex(currentVertex));

    for(TimedEdge edge : graph.getOutgoing(currentVertex))
    {
      double value = SCIPgetSolVal(scip, solution, variables(edge));

      if(value >= 0.5)
      {

        Log(debug) << "Walking from "
                   << graph.underlyingVertex(edge.getSource()).getIndex()
                   << " to "
                   << graph.underlyingVertex(edge.getTarget()).getIndex()
                   << ", starting at "
                   << edge.getSource().getTime()
                   << ", arriving at "
                   << edge.getTarget().getTime();

        currentVertex = edge.getTarget();
        found = true;

        break;
      }
    }

    if(!found)
    {
      break;
    }
  }

  assert(tourVertices.size() == vertices.size() + 1);

  tourVertices.resize(vertices.size());

  Tour tour(initialTour.getGraph(), tourVertices);

  assert(tour.connects(vertices));

  return tour;
}

void SimpleProgram::createCoveringConstraints(SCIP* scip)
{
  for(const Vertex& vertex : originalGraph.getVertices())
  {
    std::ostringstream namebuf;

    SCIP_CONS* cons;

    namebuf << "outgoing_" << vertex;

    SCIP_CALL_EXC(SCIPcreateConsLinear(scip,
                                       &cons,
                                       namebuf.str().c_str(),
                                       0,
                                       NULL,
                                       NULL,
                                       1.0, //lhs
                                       1.0, //rhs
                                       TRUE, TRUE, TRUE, TRUE, TRUE,
                                       FALSE, FALSE, FALSE, FALSE, FALSE));

    for(const TimedVertex& timedVertex : graph.getExpandedVertices(vertex))
    {
      for(TimedEdge timedEdge : graph.getOutgoing(timedVertex))
      {
        SCIP_CALL_EXC(SCIPaddCoefLinear(scip, cons, variables(timedEdge), 1.0));
      }
    }

    SCIP_CALL_EXC(SCIPaddCons(scip, cons));
    SCIP_CALL_EXC(SCIPreleaseCons(scip, &cons));
  }
}

EdgeSolutionValues SimpleProgram::getSolutionValues()
{
  return EdgeSolutionValues(scip, variables.getValues());
}
