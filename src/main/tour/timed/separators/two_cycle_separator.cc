#include "two_cycle_separator.hh"

#include <sstream>

#include "scip_utils.hh"

#include "log.hh"

#define NAME "Two-cycle separator"

TwoCycleSeparator::TwoCycleSeparator(SimpleProgram& program)
  : scip::ObjSepa(program.getSCIP(),
                  NAME,
                  "Separates two-cycle inequalities",
                  100000,
                  0,
                  0.0,
                  FALSE,
                  FALSE),
  program(program)
{
}

std::string TwoCycleSeparator::getName() const
{
  return NAME;
}

SCIP_DECL_SEPAEXECLP(TwoCycleSeparator::scip_execlp)
{
  Log(info) << "Starting 2-cycle separation";
  idx numCuts = 0;

  const TimeExpandedGraph& graph = program.getGraph();
  const EdgeMap<SCIP_VAR*>& variables = program.getVariables();

  const Vertex& originalSource = program.getInitialTour().getVertices().front();

  for(const TimedEdge& incoming : graph.getEdges())
  {
    const TimedVertex& source = incoming.getSource();
    const TimedVertex& target = incoming.getTarget();

    if(graph.underlyingVertex(target) == originalSource)
    {
      continue;
    }

    double incomingValue = SCIPgetVarSol(scip, variables(incoming));
    double outgoingValue = 0;

    if(abs(incomingValue) <= 1e-10)
    {
      incomingValue = 0;
    }

    assert(incomingValue >= 0);

    for(const TimedEdge& outgoing : graph.getOutgoing(target))
    {
      if(graph.underlyingVertex(outgoing.getTarget()) == graph.underlyingVertex(source))
      {
        continue;
      }

      outgoingValue += SCIPgetVarSol(scip, variables(outgoing));
    }

    if(outgoingValue < incomingValue - 1e-10)
    {
      ++numCuts;
      generateCut(scip, sepa, incoming);
    }
  }

  Log(info) << "Found " << numCuts << " cuts";

  *result = (numCuts > 0) ? SCIP_SEPARATED : SCIP_DIDNOTFIND;

  return SCIP_OKAY;
}

void TwoCycleSeparator::generateCut(SCIP* scip,
                                    SCIP_SEPA* sepa,
                                    const TimedEdge& incoming)
{
  SCIP_ROW* cut;

  std::ostringstream namebuf;

  const TimeExpandedGraph& graph = program.getGraph();
  const EdgeMap<SCIP_VAR*>& variables = program.getVariables();

  namebuf << "two_cycle_"
          << graph.underlyingVertex(incoming.getSource()).getIndex()
          << "_"
          << graph.underlyingVertex(incoming.getTarget()).getIndex()
          << "_"
          << incoming.getTarget().getTime();

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),
                                       -SCIPinfinity(scip),
                                       0,
                                       FALSE,
                                       FALSE,
                                       TRUE));

  SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(incoming), 1.));

  const TimedVertex& vertex = incoming.getTarget();

  bool hasOutgoing = false;

  const Vertex& originalSource = graph.underlyingVertex(incoming.getSource());

  for(const TimedEdge& outgoing : graph.getOutgoing(vertex))
  {
    if(graph.underlyingVertex(outgoing.getTarget()) == originalSource)
    {
      continue;
    }

    SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(outgoing), -1.));
    hasOutgoing = true;
  }

  assert(hasOutgoing);

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));

  /* release the row */
  SCIP_CALL_EXC(SCIPreleaseRow(scip, &cut));
}
