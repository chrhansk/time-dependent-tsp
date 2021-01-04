#include "sparse_odd_path_free_separator.hh"

#include <sstream>

SparseOddPathFreeCut::SparseOddPathFreeCut(SparseSeparator& separator,
                                           SCIP_SEPA* sepa,
                                           const OddPathFreeSet& set)
  : SparseCut(separator),
    set(set)
{
  createCut(sepa);
}

void SparseOddPathFreeCut::createCut(SCIP_SEPA* sepa)
{
  SCIP* scip = separator.getSCIP();

  std::ostringstream namebuf;

  const idx k = set.getOriginalVertices().size();

  namebuf << "sparse_odd_path_free_" << k;

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),
                                       -SCIPinfinity(scip),
                                       (k - 1) / 2.,
                                       FALSE,
                                       TRUE,
                                       TRUE));

  for(const TimedEdge& edge : set.getEdges())
  {
    SCIP_VAR* var = variables(edge);

    assert(var);

    SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, var, 1.));
  }

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));
}

void SparseOddPathFreeCut::addDualCosts(EdgeMap<double>& dualCosts, DualCostType costType) const
{
  const double costCoefficient = getDual(costType);

  for(const Edge& edge : set.getEdges())
  {
    dualCosts(edge) += costCoefficient;
  }
}


std::vector<std::unique_ptr<SparseCut>> SparseOddPathFreeSeparator::separate(const EdgeFunc<double>& values,
                                                                             SCIP_SEPA* sepa,
                                                                             int maxNumCuts)
{
  std::vector<std::unique_ptr<SparseCut>> cuts;

  SparseSolutionValues solutionValues = program.solutionValues();

  auto sets = separator.separate(solutionValues, maxNumCuts);

  for(const auto& set : sets)
  {
    cuts.push_back(std::make_unique<SparseOddPathFreeCut>(*this, sepa, set));
  }

  return cuts;
}
