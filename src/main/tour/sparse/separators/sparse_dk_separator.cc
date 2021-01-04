#include "sparse_dk_separator.hh"

#include <sstream>

SparseDKCut::SparseDKCut(SparseSeparator& separator,
                         const EdgeMap<num>& originalFactors,
                         SCIP_SEPA* sepa,
                         idx k)
  : SparseCut(separator),
    graph(separator.getProgram().getGraph()),
    originalGraph(graph.underlyingGraph()),
    originalFactors(originalFactors),
    k(k)
{
  assert(k >= 2);
  createCut(sepa);
}

void SparseDKCut::createCut(SCIP_SEPA* sepa)
{
  SCIP* scip = separator.getSCIP();

  std::ostringstream namebuf;

  namebuf << "sparse_dk_" << k;

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),
                                       -SCIPinfinity(scip),
                                       (k - 1),
                                       FALSE,
                                       TRUE,
                                       TRUE));

  for(const Edge& originalEdge : originalGraph.getEdges())
  {
    idx originalFactor = originalFactors(originalEdge);

    if(!originalFactor)
    {
      continue;
    }

    for(const TimedEdge& timedEdge : graph.getTimedEdges(originalEdge))
    {
      SCIP_VAR* var = variables(timedEdge);

      if(var)
      {
        SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, var, originalFactor));
      }
    }
  }

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));
}

void SparseDKCut::addedEdge(const TimedEdge& timedEdge)
{
  SCIP* scip = separator.getSCIP();
  const Edge& originalEdge = graph.underlyingEdge(timedEdge);

  idx originalFactor = originalFactors(originalEdge);

  if(originalFactor)
  {
    SCIP_CALL_EXC(SCIPaddVarToRow(scip,
                                  cut,
                                  variables(timedEdge),
                                  originalFactor));
  }
}

void SparseDKCut::addDualCosts(EdgeMap<double>& dualCosts, DualCostType costType) const
{
  const double costCoefficient = getDual(costType);

  for(const Edge& originalEdge : originalGraph.getEdges())
  {
    idx originalFactor = originalFactors(originalEdge);

    for(const TimedEdge& timedEdge : graph.getTimedEdges(originalEdge))
    {
      dualCosts(timedEdge) += originalFactor * costCoefficient;
    }
  }
}

std::vector<std::unique_ptr<SparseCut>>
SparseDKSeparator::separate(const EdgeFunc<double>& values,
                            SCIP_SEPA* sepa,
                            int maxNumCuts)
{
  std::vector<std::unique_ptr<SparseCut>> cuts;

  EdgeMap<double> combinedFlow = graph.combinedValues(program.solutionValues());

  auto cycles = separator.separate(combinedFlow.getValues(), maxNumCuts);

  for(const auto& cycle : cycles)
  {
    VertexMap<num> indices = separator.computeIndices(cycle);

    EdgeMap<num> originalFactors = separator.computeFactors(cycle, indices.getValues());

    cuts.push_back(std::make_unique<SparseDKCut>(*this,
                                                 originalFactors,
                                                 sepa,
                                                 cycle.size()));

  }

  return cuts;
}
