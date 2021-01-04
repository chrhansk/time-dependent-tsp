#include "simple_odd_cat_separator.hh"

#include <sstream>

#define NAME "simple_odd_cat_separator"

SimpleOddCATSeparator::SimpleOddCATSeparator(SimpleProgram& program)
  : scip::ObjSepa(program.getSCIP(),
                  NAME,
                  "Separates odd CAT inequalities",
                  100000,
                  0,
                  0.0,
                  TRUE,
                  FALSE),
  program(program),
  graph(program.getGraph()),
  originalGraph(graph.underlyingGraph()),
  separator(originalGraph)
{}

void SimpleOddCATSeparator::generateCut(SCIP* scip,
                 SCIP_SEPA* sepa,
                 const std::vector<Edge>& originalCycle)
{
  const idx k = originalCycle.size();

  assert(k >= 2);
  assert((k % 2) == 1);

  std::ostringstream namebuf;
  SCIP_ROW* cut;

  namebuf << "simple_odd_cat_" << k;

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),
                                       -SCIPinfinity(scip),
                                       (k - 1) / 2.,
                                       FALSE,
                                       FALSE,
                                       TRUE));

  for(const Edge& originalEdge : originalCycle)
  {
    addEdgeToCut(cut, originalEdge);
  }

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));

  /* release the row */
  SCIP_CALL_EXC(SCIPreleaseRow(scip, &cut));
}

void SimpleOddCATSeparator::addEdgeToCut(SCIP_ROW* cut,
                  const Edge& edge,
                  double factor)
{
  SCIP* scip = program.getSCIP();
  const EdgeMap<SCIP_VAR*>& variables = program.getVariables();

  for(const TimedEdge& timedEdge : graph.getTimedEdges(edge))
  {
    SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(timedEdge), factor));
  }
}

SCIP_DECL_SEPAEXECLP(SimpleOddCATSeparator::scip_execlp)
{
  const EdgeMap<SCIP_VAR*>& variables = program.getVariables();

  EdgeSolutionValues solutionValues(scip, variables.getValues());
  EdgeMap<double> combinedFlow = graph.combinedValues(solutionValues);

  auto cycles = separator.separate(combinedFlow.getValues());

  const idx numCuts = cycles.size();

  for(const auto& cycle : cycles)
  {
    generateCut(scip, sepa, cycle);
  }

  Log(info) << "Found " << numCuts << " cycles";

  *result = (numCuts > 0) ? SCIP_SEPARATED : SCIP_DIDNOTFIND;

  return SCIP_OKAY;
}
