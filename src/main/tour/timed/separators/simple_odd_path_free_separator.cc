#include "simple_odd_path_free_separator.hh"

#include <sstream>

#define NAME "simple_odd_path_free_separator"

SimpleOddPathFreeSeparator::SimpleOddPathFreeSeparator(SimpleProgram& program)
  : scip::ObjSepa(program.getSCIP(),
                  NAME,
                  "Separates odd path free inequalities",
                  100000,
                  0,
                  0.0,
                  TRUE,
                  FALSE),
  program(program),
  graph(program.getGraph()),
  originalGraph(graph.underlyingGraph()),
  variables(program.getVariables()),
  separator(graph, program.getSource())
{}

void
SimpleOddPathFreeSeparator::generateCut(SCIP* scip,
                                        SCIP_SEPA* sepa,
                                        const OddPathFreeSet& set)
{
  const idx k = set.getOriginalVertices().size();

  assert(k >= 1);
  assert((k % 2) == 1);

  std::ostringstream namebuf;
  SCIP_ROW* cut;

  namebuf << "simple_odd_path_free_" << k;

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),
                                       -SCIPinfinity(scip),
                                       (k - 1) / 2.,
                                       FALSE,
                                       FALSE,
                                       TRUE));

  for(const TimedEdge& edge : set.getEdges())
  {
    SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(edge), 1.));
  }

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));

  /* release the row */
  SCIP_CALL_EXC(SCIPreleaseRow(scip, &cut));

}

SCIP_DECL_SEPAEXECLP(SimpleOddPathFreeSeparator::scip_execlp)
{
  const EdgeMap<SCIP_VAR*>& variables = program.getVariables();

  EdgeSolutionValues solutionValues(scip, variables.getValues());

  std::vector<OddPathFreeSet> sets = separator.separate(solutionValues);

  const idx numCuts = sets.size();

  for(const auto& set : sets)
  {
    generateCut(scip, sepa, set);
  }

  Log(info) << "Found " << numCuts << " odd path free sets";

  *result = (numCuts > 0) ? SCIP_SEPARATED : SCIP_DIDNOTFIND;

  return SCIP_OKAY;
}
