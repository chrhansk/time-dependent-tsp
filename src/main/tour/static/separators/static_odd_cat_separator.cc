#include "static_odd_cat_separator.hh"

#include <sstream>

#include "scip_utils.hh"

#define NAME "static_odd_cat_separator"

StaticOddCATSeparator::StaticOddCATSeparator(TourSolver& solver)
  : scip::ObjSepa(solver.getSCIP(),
                  NAME,
                  "Separates odd CAT inequalities",
                  100000,
                  0,
                  0.0,
                  TRUE,
                  FALSE),
  solver(solver),
  graph(solver.getGraph()),
  separator(graph)
{

}

void StaticOddCATSeparator::generateCut(SCIP* scip,
                                        SCIP_SEPA* sepa,
                                        const std::vector<Edge>& cycle)
{
  const idx k = cycle.size();

  const EdgeMap<SCIP_VAR*>& variables = solver.getVariables();

  assert(k >= 2);
  assert((k % 2) == 1);

  std::ostringstream namebuf;
  SCIP_ROW* cut;

  namebuf << "static_odd_cat_" << k;

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),
                                       -SCIPinfinity(scip),
                                       (k - 1) / 2.,
                                       FALSE,
                                       FALSE,
                                       TRUE));

  for(const Edge& edge : cycle)
  {
    SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(edge), 1.));
  }

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));

  /* release the row */
  SCIP_CALL_EXC(SCIPreleaseRow(scip, &cut));
}

SCIP_DECL_SEPAEXECLP(StaticOddCATSeparator::scip_execlp)
{
  const EdgeMap<SCIP_VAR*>& variables = solver.getVariables();

  EdgeSolutionValues solutionValues(scip, variables.getValues());

  auto cycles = separator.separate(solutionValues);

  const idx numCuts = cycles.size();

  for(const auto& cycle : cycles)
  {
    generateCut(scip, sepa, cycle);
  }

  Log(info) << "Found " << numCuts << " cycles";

  *result = (numCuts > 0) ? SCIP_SEPARATED : SCIP_DIDNOTFIND;

  return SCIP_OKAY;
}
