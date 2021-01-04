#ifndef SPARSE_SUBTOUR_SEPARATOR_HH
#define SPARSE_SUBTOUR_SEPARATOR_HH

#include "sparse_separator.hh"

class SparseSubTourCut : public SparseCut
{
private:
  const TimeExpandedGraph& graph;
  const Graph& originalGraph;
  VertexSet vertices;

  void generateCut(SCIP_SEPA* sepa);

public:
  SparseSubTourCut(SparseSeparator& separator,
                   const VertexSet& vertices,
                   SCIP_SEPA* sepa);

  void addedEdge(const TimedEdge& timedEdge) override;

  void addDualCosts(EdgeMap<double>& dualCosts, DualCostType costType) const override;
};


class SparseSubtourSeparator : public SparseSeparator
{
private:
  const TimeExpandedGraph& graph;
public:
  SparseSubtourSeparator(SparseProgram& program)
    : SparseSeparator(program),
      graph(program.getGraph())
  {}

  std::vector<std::unique_ptr<SparseCut>> separate(const EdgeFunc<double>& values,
                                                   SCIP_SEPA* sepa,
                                                   int maxNumCuts = -1) override;
};


#endif /* SPARSE_SUBTOUR_SEPARATOR_HH */
