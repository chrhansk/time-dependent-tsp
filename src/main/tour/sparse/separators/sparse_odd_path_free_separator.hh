#ifndef SPARSE_ODD_PATH_FREE_SEPARATOR_HH
#define SPARSE_ODD_PATH_FREE_SEPARATOR_HH

#include "sparse_separator.hh"

#include "tour/separators/odd_path_free_separator.hh"

class SparseOddPathFreeCut : public SparseCut
{
private:
  OddPathFreeSet set;

  void createCut(SCIP_SEPA* sepa);

public:
  SparseOddPathFreeCut(SparseSeparator& separator,
                       SCIP_SEPA* sepa,
                       const OddPathFreeSet& set);

  void addedEdge(const TimedEdge& timedEdge) override
  {}

  void addDualCosts(EdgeMap<double>& dualCosts, DualCostType costType) const override;

};


class SparseOddPathFreeSeparator : public SparseSeparator
{
private:
  const TimeExpandedGraph& graph;
  const Graph& originalGraph;
  OddPathFreeSeparator separator;

public:
  SparseOddPathFreeSeparator(SparseProgram& program)
    : SparseSeparator(program),
      graph(program.getGraph()),
      originalGraph(graph.underlyingGraph()),
      separator(graph, program.getSource())
  {}

  std::vector<std::unique_ptr<SparseCut>> separate(const EdgeFunc<double>& values,
                                                   SCIP_SEPA* sepa,
                                                   int maxNumCuts = -1) override;
};


#endif /* SPARSE_ODD_PATH_FREE_SEPARATOR_HH */
