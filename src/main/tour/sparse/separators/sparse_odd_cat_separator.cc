#include "sparse_odd_cat_separator.hh"

#include <sstream>

SparseOddCATCut::SparseOddCATCut(SparseSeparator& separator,
                                 const std::vector<Edge>& originalCycle,
                                 SCIP_SEPA* sepa)
  : SparseCut(separator),
    graph(separator.getProgram().getGraph()),
    originalGraph(graph.underlyingGraph()),
    originalCycle(originalCycle)
{
  createCut(sepa);
}

void SparseOddCATCut::createCut(SCIP_SEPA* sepa)
{
  const idx k = originalCycle.size();

  assert(k >= 2);
  assert((k % 2) == 1);

  SCIP* scip = separator.getSCIP();

  std::ostringstream namebuf;

  namebuf << "sparse_odd_cat_" << k;

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),
                                       -SCIPinfinity(scip),
                                       (k - 1) / 2.,
                                       FALSE,
                                       TRUE,
                                       TRUE));

  for(const Edge& originalEdge : originalCycle)
  {
    for(const TimedEdge& timedEdge : graph.getTimedEdges(originalEdge))
    {
      SCIP_VAR* var = variables(timedEdge);

      if(var)
      {
        SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, var, 1.));
      }
    }
  }

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));
}

void SparseOddCATCut::addedEdge(const TimedEdge& timedEdge)
{
  SCIP* scip = separator.getSCIP();
  const Edge& originalEdge = graph.underlyingEdge(timedEdge);

  if(contains(originalCycle, originalEdge))
  {
    SCIP_CALL_EXC(SCIPaddVarToRow(scip,
                                  cut,
                                  variables(timedEdge),
                                  1.));
  }
}

void SparseOddCATCut::addDualCosts(EdgeMap<double>& dualCosts, DualCostType costType) const
{
  const double costCoefficient = getDual(costType);

  for(const Edge& originalEdge : originalCycle)
  {
    for(const TimedEdge& timedEdge : graph.getTimedEdges(originalEdge))
    {
      dualCosts(timedEdge) += costCoefficient;
    }
  }
}

std::vector<std::unique_ptr<SparseCut>>
SparseOddCATSeparator::separate(const EdgeFunc<double>& values,
                                SCIP_SEPA* sepa,
                                int maxNumCuts)
{
  std::vector<std::unique_ptr<SparseCut>> cuts;

  EdgeMap<double> combinedFlow = graph.combinedValues(program.solutionValues());

  auto cycles = separator.separate(combinedFlow.getValues(), maxNumCuts);

  for(const auto& cycle : cycles)
  {
    cuts.push_back(std::make_unique<SparseOddCATCut>(*this,
                                                     cycle,
                                                     sepa));

  }

  return cuts;
}
