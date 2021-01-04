#include "subtour_separator.hh"

#include <sstream>

#include "scip_utils.hh"

#include "log.hh"

#include "flow/max_flow.hh"

#define NAME "Subtour separator"

const double cutoff = 1e-6;

SubtourSeparator::SubtourSeparator(SimpleProgram& program)
  : scip::ObjSepa(program.getSCIP(),
                  NAME,
                  "Separates subtours",
                  100000,
                  0,
                  0.0,
                  FALSE,
                  FALSE),
  program(program),
  graph(program.getGraph())
{
}

std::string SubtourSeparator::getName() const
{
  return NAME;
}

SCIP_DECL_SEPAEXECLP(SubtourSeparator::scip_execlp)
{
  Log(info) << "Starting subtour separation";

  const EdgeMap<SCIP_VAR*>& variables = program.getVariables();

  EdgeSolutionValues solutionValues(scip, variables.getValues());
  EdgeMap<double> combinedFlow = graph.combinedValues(solutionValues);

  const TimeExpandedGraph& graph = program.getGraph();
  const Graph& originalGraph = graph.underlyingGraph();

  MinCutResult cutResult = computeMinCut(originalGraph,
                                         combinedFlow.getValues());

  if(cutResult.value >= 1 - cutoff)
  {
    Log(info) << "Could not find a cut";
  }
  else
  {
    Log(info) << "Found a cut with a value of " << cutResult.value;

    generateCut(scip, sepa, cutResult.cut);

    *result = SCIP_SEPARATED;
  }

  return SCIP_OKAY;
}

void SubtourSeparator::generateCut(SCIP* scip,
                                   SCIP_SEPA* sepa,
                                   const VertexSet& originalVertices)
{
  std::ostringstream namebuf;
  SCIP_ROW* cut;

  const EdgeMap<SCIP_VAR*>& variables = program.getVariables();
  const TimeExpandedGraph& graph = program.getGraph();

  idx numOriginalVertices = 0;

  for(const Vertex& vertex : graph.underlyingGraph().getVertices())
  {
    if(originalVertices.contains(vertex))
    {
      ++numOriginalVertices;
    }
  }

  namebuf << "subtour_elimination_" << numOriginalVertices;

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),
                                       1,
                                       SCIPinfinity(scip),
                                       FALSE,
                                       FALSE,
                                       TRUE));

  idx numEdges = 0;

  for(const TimedEdge& edge : graph.getEdges())
  {
    Vertex originalSource = graph.underlyingVertex(edge.getSource());
    Vertex originalTarget = graph.underlyingVertex(edge.getTarget());

    if(originalVertices.contains(originalSource) &&
       !originalVertices.contains(originalTarget))
    {
      SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(edge), 1.));
      ++numEdges;
    }
  }

  assert(numEdges > 0);

  Log(info) << "Created a cut with " << numEdges << " edges";

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));

  /* release the row */
  SCIP_CALL_EXC(SCIPreleaseRow(scip, &cut));
}
