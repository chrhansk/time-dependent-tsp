#ifndef TIME_GREEDY_CONSTRUCTION_HH
#define TIME_GREEDY_CONSTRUCTION_HH

#include "greedy_construction.hh"

class TimeGreedyConstruction : public GreedyConstruction
{
protected:
  EdgeMap<double> combinedValues;

  virtual double score(const TimedEdge& edge) const override;

public:
  TimeGreedyConstruction(SparseProgram& program,
                         int numRuns = 50);
};


#endif /* TIME_GREEDY_CONSTRUCTION_HH */
