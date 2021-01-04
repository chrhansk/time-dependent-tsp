#ifndef SPARSE_STABILIZING_PRICER_HH
#define SPARSE_STABILIZING_PRICER_HH

#include "sparse_path_pricer.hh"

class SparseStabilizingPricer : public SparsePricer
{
private:
  std::unique_ptr<SparsePathPricer> sparsePricer;
  const Graph& originalGraph;

  double proveGap;
  static const double initialProveGap;

  class DualValues : public EdgeFunc<double>
  {
  private:
    std::optional<double> lagrangianBound;
    double offset;
    EdgeMap<double> dualValues;

  public:
    DualValues(const EdgeMap<double>& dualValues,
               std::optional<double> lagrangianBound = 0.,
               double offset = 0.)
      : lagrangianBound(lagrangianBound),
        offset(offset),
        dualValues(dualValues)
    {}

    DualValues(const DualValues& other) = default;

    DualValues& operator=(const DualValues& other) = default;

    double operator()(const Edge& edge) const override
    {
      return dualValues(edge);
    }

    double getOffset() const
    {
      return offset;
    }

    void setOffset(double value)
    {
      offset = value;
    }

    std::optional<double> getLagrangianBound() const
    {
      return lagrangianBound;
    }

    void setLagrangianBound(double value)
    {
      lagrangianBound = value;
    }
  };

  DualValues centeredDualValues;

  std::vector<TimedPath> findPaths(const EdgeFunc<double>& reducedCosts,
                                   double& minReducedCost);

  DualValues getCurrentDualValues();

  void checkUpperBound();

  void checkDualFeasibility();

public:
  SparseStabilizingPricer(SparseProgram& program,
                          std::unique_ptr<SparsePathPricer>&& sparsePricer);

  virtual SparsePricingResult performPricing(DualCostType costType) override;
};


#endif /* SPARSE_STABILIZING_PRICER_HH */
