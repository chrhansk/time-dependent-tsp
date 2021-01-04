#include "sparse_cut.hh"

#include "tour/sparse/pricers/sparse_pricing_manager.hh"

#include "sparse_separator.hh"

SparseCut::SparseCut(SparseSeparator& separator)
  : separator(separator),
    variables(separator.getProgram().getPricingManager().getVariables()),
    cut(nullptr)
{}

SparseCut::~SparseCut()
{
  SCIP_CALL_ASSERT(SCIPreleaseRow(separator.getSCIP(), &cut));
}

double SparseCut::getDual(DualCostType costType) const
{
  assert(cut);

  if(costType == DualCostType::FARKAS)
  {
    return SCIProwGetDualfarkas(cut);
  }
  else
  {
    assert(costType == DualCostType::SIMPLE);

    return SCIProwGetDualsol(cut);
  }
}
