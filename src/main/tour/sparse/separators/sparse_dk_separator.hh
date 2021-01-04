#ifndef SPARSE_DK_SEPARATOR_HH
#define SPARSE_DK_SEPARATOR_HH

#include "sparse_separator.hh"

#include "tour/separators/dk_separator.hh"

class SparseDKCut : public SparseCut
{
private:
  const TimeExpandedGraph& graph;
  const Graph& originalGraph;
  EdgeMap<num> originalFactors;
  idx k;

  void createCut(SCIP_SEPA* sepa);

public:
  SparseDKCut(SparseSeparator& separator,
              const EdgeMap<num>& originalFactors,
              SCIP_SEPA* sepa,
              idx k);

  void addedEdge(const TimedEdge& timedEdge) override;

  void addDualCosts(EdgeMap<double>& dualCosts, DualCostType costType) const override;
};

class SparseDKSeparator : public SparseSeparator
{
private:
  const TimeExpandedGraph& graph;
  const Graph& originalGraph;
  DKSeparator separator;

public:
  SparseDKSeparator(SparseProgram& program)
    : SparseSeparator(program),
      graph(program.getGraph()),
      originalGraph(graph.underlyingGraph()),
      separator(originalGraph)
  {}

  std::vector<std::unique_ptr<SparseCut>> separate(const EdgeFunc<double>& values,
                                                   SCIP_SEPA* sepa,
                                                   int maxNumCuts = -1) override;
};


#endif /* SPARSE_DK_SEPARATOR_HH */
