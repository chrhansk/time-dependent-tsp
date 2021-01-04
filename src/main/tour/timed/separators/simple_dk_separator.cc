#include "simple_dk_separator.hh"

#include <sstream>

#define NAME "simple_dk_separator"

SimpleDKSeparator::SimpleDKSeparator(SimpleProgram& program)
  : scip::ObjSepa(program.getSCIP(),
                  NAME,
                  "Separates Dk+ inequalities",
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

void SimpleDKSeparator::generateCut(SCIP* scip,
                                    SCIP_SEPA* sepa,
                                    const std::vector<Vertex>& originalVertices)
{
  const idx k = originalVertices.size();

  assert(k >= 2);
  assert(k < originalGraph.getVertices().size());

  std::ostringstream namebuf;
  SCIP_ROW* cut;

  namebuf << "simple_dk_" << k;

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),
                                       -SCIPinfinity(scip),
                                       (k - 1),
                                       FALSE,
                                       FALSE,
                                       TRUE));

  VertexMap<num> indices = separator.computeIndices(originalVertices);

  if(debuggingEnabled())
  {
    VertexSet vertexSet(originalGraph);

    for(const Vertex& originalVertex : originalVertices)
    {
      assert(!vertexSet.contains(originalVertex));
      vertexSet.insert(originalVertex);
    }
  }

  EdgeMap<num> factors = separator.computeFactors(originalVertices,
                                                  indices.getValues());

  for(const Edge& originalEdge : originalGraph.getEdges())
  {
    idx factor = factors(originalEdge);

    if(factor)
    {
      addEdgeToCut(cut, originalEdge, factor);
    }
  }

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));

  /* release the row */
  SCIP_CALL_EXC(SCIPreleaseRow(scip, &cut));

}

void SimpleDKSeparator::addEdgeToCut(SCIP_ROW* cut,
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


SCIP_DECL_SEPAEXECLP(SimpleDKSeparator::scip_execlp)
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
