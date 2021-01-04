#ifndef LP_OBSERVER_HH
#define LP_OBSERVER_HH

#include <objscip/objscip.h>

#include "timer.hh"
#include "util.hh"
#include "solution_stats.hh"

class LPObserver : public scip::ObjEventhdlr
{
private:
  SCIP* scip;

  idx numLPs;
  idx numRows;
  idx numCols;

  idx maxRows;
  idx maxCols;

  idx minRows;
  idx minCols;

  double LProotObjVal;
  bool rootSolved;
  double rootSolvingTime;

  Timer rootTimer;

public:
  LPObserver(SCIP* scip);

  virtual SCIP_DECL_EVENTINITSOL(scip_initsol) override;

  virtual SCIP_DECL_EVENTEXEC(scip_exec) override;

  virtual SCIP_DECL_EVENTEXITSOL(scip_exitsol) override;

  virtual SCIP_DECL_EVENTEXIT (scip_exit) override;

  idx getNumLPs() const
  {
    return numLPs;
  }

  idx getNumRows() const
  {
    return numRows;
  }

  idx getNumCols() const
  {
    return numCols;
  }

  idx getMaxRows() const
  {
    if(getNumLPs() == 0)
    {
      return -1;
    }

    return maxRows;
  }

  idx getMaxCols() const
  {
    if(getNumLPs() == 0)
    {
      return -1;
    }

    return maxCols;
  }

  idx getMinRows() const
  {
    if(getNumLPs() == 0)
    {
      return -1;
    }

    return minRows;
  }

  idx getMinCols() const
  {
    if(getNumLPs() == 0)
    {
      return -1;
    }

    return minCols;
  }

  double getAvgRows() const
  {
    if(getNumLPs() == 0)
    {
      return -1;
    }

    return getNumRows() / ((double) getNumLPs());
  }

  double getAvgCols() const
  {
    if(getNumLPs() == 0)
    {
      return -1;
    }

    return getNumCols() / ((double) getNumLPs());
  }

  double getLProotObjVal() const
  {
    return LProotObjVal;
  }

  bool rootLPSolved() const
  {
    return rootSolved;
  }

  double rootNodeSolvingTime() const
  {
    return rootSolvingTime;
  }

  LPStats getStats() const;

};


#endif /* LP_OBSERVER_HH */
