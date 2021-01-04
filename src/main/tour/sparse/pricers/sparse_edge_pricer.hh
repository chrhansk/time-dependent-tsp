#ifndef SPARSE_EDGE_PRICER_HH
#define SPARSE_EDGE_PRICER_HH

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <objscip/objscip.h>

#include "graph/graph.hh"
#include "tour/tour.hh"

#include "tour/sparse/sparse_program.hh"

#include "tour/sparse/pricers/sparse_pricer.hh"

class SparseEdgePricer : public SparsePricer
{
private:

  struct Candidate
  {
    TimedEdge timedEdge;
    double cost;
  };

  const Graph& originalGraph;

  idx numEdges;

  EdgeMap<std::vector<Candidate>> findCandidates(const EdgeFunc<double>& costs,
                                                 std::optional<double>& minReducedCost);

public:
  SparseEdgePricer(SparseProgram& program,
                   idx numEdges = 20);

  virtual SparsePricingResult performPricing(DualCostType costType) override;
};


#endif /* SPARSE_EDGE_PRICER_HH */
