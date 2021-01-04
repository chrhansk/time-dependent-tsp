#ifndef SPARSE_ODD_CAT_SEPARATOR_HH
#define SPARSE_ODD_CAT_SEPARATOR_HH

#include "sparse_separator.hh"

#include "tour/separators/odd_cat_separator.hh"

class SparseOddCATCut : public SparseCut
{
private:
  const TimeExpandedGraph& graph;
  const Graph& originalGraph;
  std::vector<Edge> originalCycle;

  void createCut(SCIP_SEPA* sepa);

public:
  SparseOddCATCut(SparseSeparator& separator,
                  const std::vector<Edge>& originalCycle,
                  SCIP_SEPA* sepa);

  void addedEdge(const TimedEdge& timedEdge) override;

  void addDualCosts(EdgeMap<double>& dualCosts, DualCostType costType) const override;
};

class SparseOddCATSeparator : public SparseSeparator
{
private:
  const TimeExpandedGraph& graph;
  const Graph& originalGraph;
  OddCATSeparator separator;

public:
  SparseOddCATSeparator(SparseProgram& program)
    : SparseSeparator(program),
      graph(program.getGraph()),
      originalGraph(graph.underlyingGraph()),
      separator(originalGraph)
  {}

  std::vector<std::unique_ptr<SparseCut>> separate(const EdgeFunc<double>& values,
                                                   SCIP_SEPA* sepa,
                                                   int maxNumCuts = -1) override;
};

#endif /* SPARSE_ODD_CAT_SEPARATOR_HH */
