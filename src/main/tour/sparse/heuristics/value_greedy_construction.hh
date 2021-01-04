#ifndef VALUE_GREEDY_CONSTRUCTION_HH
#define VALUE_GREEDY_CONSTRUCTION_HH

#include "greedy_construction.hh"

class ValueGreedyConstruction : public GreedyConstruction
{
protected:
  virtual double score(const TimedEdge& edge) const override;

public:
  ValueGreedyConstruction(SparseProgram& program,
                          int numRuns = 50);
};


#endif /* VALUE_GREEDY_CONSTRUCTION_HH */
