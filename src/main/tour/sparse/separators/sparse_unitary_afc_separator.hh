#ifndef SPARSE_UNITARY_AFC_SEPARATOR_HH
#define SPARSE_UNITARY_AFC_SEPARATOR_HH

#include "sparse_cut.hh"
#include "sparse_separator.hh"

#include "tour/separators/unitary_afc_separator.hh"

class SparseUnitaryAFCCut : public SparseCut
{
private:
  const TimeExpandedGraph& graph;
  TimedEdge incoming;
  VertexSet vertices;

  Vertex originalSource, originalTarget;

  void createCut(SCIP_SEPA* sepa);

public:
  SparseUnitaryAFCCut(SparseSeparator& separator,
                      const TimeExpandedGraph& graph,
                      TimedEdge incoming,
                      VertexSet vertices,
                      SCIP_SEPA* sepa);

  void addedEdge(const TimedEdge& timedEdge) override;

  bool validEdge(const TimedEdge& timedEdge) const;

  void addDualCosts(EdgeMap<double>& dualCosts, DualCostType costType) const override;
};

class SparseUnitaryAFCSeparator : public SparseSeparator
{
private:
  const TimeExpandedGraph& graph;
  UnitaryAFCSeparator separator;

public:
  SparseUnitaryAFCSeparator(SparseProgram& program)
    : SparseSeparator(program),
      graph(program.getGraph()),
      separator(program.getGraph(), program.getSource())
  {}

  std::vector<std::unique_ptr<SparseCut>> separate(const EdgeFunc<double>& values,
                                                   SCIP_SEPA* sepa,
                                                   int maxNumSets = -1) override;

};


#endif /* SPARSE_UNITARY_AFC_SEPARATOR_HH */
