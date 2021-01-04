#include "tour_solver.hh"

#include <sstream>

#include <scip/scipdefplugins.h>

#include "scip_utils.hh"
#include "subtour_handler.hh"

TourSolver::TourSolver(const Graph& graph,
                       DistanceFunc& distances,
                       const Program::Settings& settings)
  : graph(graph),
    distances(distances),
    program("static_tour", settings),
    scip(program.getSCIP()),
    variables(graph, nullptr)
{
}

void TourSolver::createVariables(SCIP* scip)
{
  std::ostringstream namebuf;

  for(const Edge& edge : graph.getEdges())
  {
    const num cost = distances(edge.getSource(), edge.getTarget());
    SCIP_VAR* var;

    namebuf.str("");
    namebuf << "x_" << edge.getSource().getIndex()
            << "_" << edge.getTarget().getIndex();

    SCIP_CALL_EXC(SCIPcreateVar(scip,
                                &var,
                                namebuf.str().c_str(),
                                0.0, 1.0,
                                cost,
                                SCIP_VARTYPE_BINARY,
                                TRUE,
                                FALSE,
                                NULL, NULL, NULL, NULL, NULL));

    SCIP_CALL_EXC(SCIPaddVar(scip, var));

    variables(edge) = var;
  }
}

void TourSolver::createFlowConstraints(SCIP* scip)
{
  std::ostringstream namebuf;

  for(const Vertex& vertex : graph.getVertices())
  {
    SCIP_CONS* cons;
    namebuf.str("");
    namebuf << "outgoing_" << vertex.getIndex();

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

    for(const Edge& edge : graph.getOutgoing(vertex))
    {
      SCIP_CALL_EXC(SCIPaddCoefLinear(scip, cons, variables(edge), 1.0));
    }

    SCIP_CALL_EXC(SCIPaddCons(scip, cons));
    SCIP_CALL_EXC(SCIPreleaseCons(scip, &cons));
  }

  for(const Vertex& vertex : graph.getVertices())
  {
    SCIP_CONS* cons;
    namebuf.str("");
    namebuf << "incoming_" << vertex.getIndex();

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

    for(const Edge& edge : graph.getIncoming(vertex))
    {
      SCIP_CALL_EXC(SCIPaddCoefLinear(scip, cons, variables(edge), 1.0));
    }

    SCIP_CALL_EXC(SCIPaddCons(scip, cons));
    SCIP_CALL_EXC(SCIPreleaseCons(scip, &cons));
  }

}

void TourSolver::addSubtourConstraint(const std::string& handlerName)
{
  SCIP_CONSHDLR* conshdlr = SCIPfindConshdlr(scip, handlerName.c_str());

  assert(conshdlr != NULL);

  SCIP_CONS* cons;

  SCIP_CALL_EXC(SCIPcreateCons(scip,
                               &cons,
                               handlerName.c_str(),
                               conshdlr,
                               NULL,
                               FALSE, TRUE, TRUE, TRUE,
                               TRUE, FALSE, FALSE, FALSE,
                               TRUE, FALSE));

  SCIP_CALL_EXC(SCIPaddCons(scip, cons));
  SCIP_CALL_EXC(SCIPreleaseCons(scip, &cons));
}

Tour TourSolver::findTour()
{
  SubtourHandler* handler = new SubtourHandler(scip,
                                               graph,
                                               variables);

  createVariables(scip);

  createFlowConstraints(scip);

  SCIP_CALL_EXC(SCIPincludeObjConshdlr(scip, handler, TRUE));

  addSubtourConstraint(handler->getName());

  Log(info) << "Created subtour handler";

  SCIP_CALL_EXC(SCIPsolve(scip));

  SCIP_SOL* solution = SCIPgetBestSol(scip);

  if(!solution)
  {
    throw std::invalid_argument("No solution found");
  }

  return createTour(solution);
}

Tour TourSolver::createTour(SCIP_SOL* solution)
{
  Vertex current = graph.getVertices().collect().front();

  std::vector<Vertex> vertices;
  vertices.reserve(vertices.size());

  idx numVertices = graph.getVertices().size();
  idx currentCount = 0;

  while(true)
  {
    vertices.push_back(current);

    if(currentCount + 1 == numVertices)
    {
      break;
    }

    ++currentCount;

    bool found = false;

    for(const Edge& edge : graph.getOutgoing(current))
    {
      double value = SCIPgetSolVal(scip, solution, variables(edge));

      if(value >= 0.5)
      {
        Vertex next = edge.getTarget();

        Log(info) << "Walking from "
                  << current.getIndex()
                  << " to "
                  << next.getIndex();

        current = next;
        found = true;

        break;
      }
    }

    assert(found);
  }

  Tour tour(graph, vertices);

  assert(tour.connects(graph.getVertices().collect()));

  return tour;
}

void TourSolver::addSeparator(scip::ObjSepa* separator)
{
  SCIP_CALL_EXC(SCIPincludeObjSepa(scip, separator, TRUE));
}


TourSolver::~TourSolver()
{
  for(const Edge& edge : graph.getEdges())
  {
    SCIP_CALL_ASSERT(SCIPreleaseVar(scip, &variables(edge)));
  }

  SCIP_CALL_ASSERT(SCIPfree(&scip));
}
