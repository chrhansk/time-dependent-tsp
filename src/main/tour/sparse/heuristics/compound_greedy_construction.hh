#ifndef COMPOUND_GREEDY_CONSTRUCTION_HH
#define COMPOUND_GREEDY_CONSTRUCTION_HH

#include "greedy_construction.hh"

class CompoundGreedyConstruction : public GreedyConstruction
{
protected:
  EdgeMap<double> combinedValues;

  virtual double score(const TimedEdge& edge) const override;

  virtual bool startRound() override;

public:
  CompoundGreedyConstruction(SparseProgram& program,
                             int numRuns = 50);
};


#endif /* COMPOUND_GREEDY_CONSTRUCTION_HH */
