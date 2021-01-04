#include "static_dk_separator.hh"

#include <sstream>

#include "scip_utils.hh"

#define NAME "static_dk_separator"

StaticDKSeparator::StaticDKSeparator(TourSolver& solver)
  : scip::ObjSepa(solver.getSCIP(),
                  NAME,
                  "Separates Dk+ inequalities",
                  100000,
                  0,
                  0.0,
                  TRUE,
                  FALSE),
  solver(solver),
  graph(solver.getGraph()),
  separator(graph)
{}

void StaticDKSeparator::generateCut(SCIP* scip,
                                    SCIP_SEPA* sepa,
                                    const std::vector<Vertex>& vertices)
{
  const idx k = vertices.size();

  const EdgeMap<SCIP_VAR*>& variables = solver.getVariables();

  assert(k >= 2);

  std::ostringstream namebuf;
  SCIP_ROW* cut;

  namebuf << "static_dk_" << k;

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),
                                       -SCIPinfinity(scip),
                                       (k - 1),
                                       FALSE,
                                       FALSE,
                                       TRUE));

  VertexMap<num> indices = separator.computeIndices(vertices);

  if(debuggingEnabled())
  {
    VertexSet vertexSet(graph);

    for(const Vertex& vertex : vertices)
    {
      assert(!vertexSet.contains(vertex));
      vertexSet.insert(vertex);
    }
  }

  EdgeMap<num> factors = separator.computeFactors(vertices,
                                                  indices.getValues());

  for(const Edge& edge : graph.getEdges())
  {
    idx factor = factors(edge);

    if(factor)
    {
      SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(edge), factor));
    }
  }

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));

  /* release the row */
  SCIP_CALL_EXC(SCIPreleaseRow(scip, &cut));
}

SCIP_DECL_SEPAEXECLP(StaticDKSeparator::scip_execlp)
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
