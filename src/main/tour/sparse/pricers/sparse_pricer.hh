#ifndef SPARSE_PRICER_HH
#define SPARSE_PRICER_HH

#include "tour/sparse/sparse_program.hh"
#include "sparse_pricing_result.hh"

class SparsePricingManager;

class SparsePricer
{
protected:
  class ReducedCosts : public EdgeFunc<double>
  {
    const EdgeFunc<double>& dualCosts;
    const EdgeFunc<num>& travelTimes;

  public:
    ReducedCosts(const EdgeFunc<double>& dualCosts,
                 const EdgeFunc<num>& travelTimes)
      : dualCosts(dualCosts),
        travelTimes(travelTimes)
    {}

    double operator()(const Edge& edge) const override
    {
      return travelTimes(edge) - dualCosts(edge);
    }
  };

  class FarkasCosts : public EdgeFunc<double>
  {
    const EdgeFunc<double>& dualCosts;

  public:
    FarkasCosts(const EdgeFunc<double>& dualCosts)
      : dualCosts(dualCosts)
    {}

    double operator()(const Edge& edge) const override
    {
      return -dualCosts(edge);
    }
  };

  SparseProgram& program;
  SparsePricingManager& manager;
  SCIP* scip;
  const TimeExpandedGraph& graph;
  Vertex source;

  const VertexMap<SCIP_CONS*>& getFlowConstraints() const;

  const EdgeMap<SCIP_CONS*>& getLinkingConstraints() const;

  const VertexMap<SCIP_CONS*>& getCoveringConstraints() const;

  const EdgeMap<SCIP_VAR*>& getVariables() const;

public:
  SparsePricer(SparseProgram& program);

  virtual SparsePricingResult performPricing(DualCostType costType) = 0;
};


#endif /* SPARSE_PRICER_HH */
