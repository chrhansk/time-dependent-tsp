#ifndef SPARSE_LIFTED_SUBTOUR_SEPARATOR_HH
#define SPARSE_LIFTED_SUBTOUR_SEPARATOR_HH

#include "sparse_cut.hh"
#include "sparse_separator.hh"

class SparseLiftedSubtourCut : public SparseCut
{
private:
  VertexSet originalVertices;
  idx maxTime;

  void createCut(SCIP_SEPA* sepa);

public:
  SparseLiftedSubtourCut(SparseSeparator& separator,
                         const VertexSet& originalVertices,
                         idx maxTime,
                         SCIP_SEPA* sepa);

  void addedEdge(const TimedEdge& timedEdge) override;

  void addDualCosts(EdgeMap<double>& dualCosts, DualCostType costType) const override;
};


class SparseLiftedSubtourSeparator : public SparseSeparator
{
private:
  const DistanceFunc& distances;
  bool subtourBound;

  idx computeMaxTime(const VertexSet& originalVertices);

public:
  SparseLiftedSubtourSeparator(SparseProgram& program,
                               const DistanceFunc& distances,
                               bool subtourBound = false)
    : SparseSeparator(program),
      distances(distances),
      subtourBound(subtourBound)
  {}

  std::vector<std::unique_ptr<SparseCut>> separate(const EdgeFunc<double>& values,
                                                   SCIP_SEPA* sepa,
                                                   int maxNumCuts = -1) override;
};


#endif /* SPARSE_LIFTED_SUBTOUR_SEPARATOR_HH */
