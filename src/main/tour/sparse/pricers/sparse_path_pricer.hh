#ifndef SPARSE_PATH_PRICER
#define SPARSE_PATH_PRICER

#include "sparse_pricer.hh"

class SparsePathPricer : public SparsePricer
{
protected:
  const idx numPaths;
public:
  SparsePathPricer(SparseProgram& program,
                   idx numPaths = 20);

  virtual SparsePricingResult performPricing(DualCostType costType) override;

  virtual std::vector<TimedPath> findPaths(const EdgeFunc<double>& reducedCosts,
                                           const EdgeSet& forbiddenEdges,
                                           const std::optional<double>& lowerTimeBound,
                                           const std::optional<double>& upperTimeBound,
                                           std::optional<double>& minReducedCost) = 0;
};


#endif /* SPARSE_PATH_PRICER */
