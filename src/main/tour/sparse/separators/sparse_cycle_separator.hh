#ifndef SPARSE_CYCLE_SEPARATOR_HH
#define SPARSE_CYCLE_SEPARATOR_HH

#include "sparse_cut.hh"
#include "sparse_separator.hh"

class SparseCycleCut : public SparseCut
{
private:
  const TimeExpandedGraph& graph;
  TimedEdge incoming;
  TimedPath cycle;
  VertexMap<num> indices;

  void addIndices();

  void createCut(SCIP_SEPA* sepa);

public:
  SparseCycleCut(SparseSeparator& separator,
                 const TimedEdge& incoming,
                 const TimedPath& cycle,
                 SCIP_SEPA* sepa);

  void addedEdge(const TimedEdge& timedEdge) override;

  void addDualCosts(EdgeMap<double>& dualCosts, DualCostType costType) const override;
};

class SparseCycleSeparator : public SparseSeparator
{
private:
  bool hasCycle(const TimedPath& path,
                TimedEdge& incoming,
                TimedPath& cycle);

public:
  SparseCycleSeparator(SparseProgram& program)
    : SparseSeparator(program)
  {}

  std::vector<std::unique_ptr<SparseCut>> separate(const EdgeFunc<double>& values,
                                                   SCIP_SEPA* sepa,
                                                   int maxNumCuts = -1) override;

};


#endif /* SPARSE_CYCLE_SEPARATOR_HH */
