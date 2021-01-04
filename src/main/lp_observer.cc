#include "lp_observer.hh"

#include "scip_utils.hh"

LPObserver::LPObserver(SCIP* scip)
  : scip::ObjEventhdlr(scip,
                       "observer",
                       "Observes the solving process"),
    scip(scip),
    numLPs(0),
    numRows(0),
    numCols(0),
    maxRows(0),
    maxCols(0),
    minRows(inf),
    minCols(inf),
    LProotObjVal(-inf),
    rootSolved(false),
    rootSolvingTime(inf)
{
}

SCIP_DECL_EVENTINITSOL(LPObserver::scip_initsol)
{
  SCIP_CALL_EXC(SCIPcatchEvent(scip, SCIP_EVENTTYPE_LPSOLVED, eventhdlr, NULL, NULL));

  SCIP_CALL_EXC(SCIPcatchEvent(scip, SCIP_EVENTTYPE_NODEFOCUSED, eventhdlr, NULL, NULL));
  SCIP_CALL_EXC(SCIPcatchEvent(scip, SCIP_EVENTTYPE_NODESOLVED, eventhdlr, NULL, NULL));

  return SCIP_OKAY;
}

// (SCIP* scip, SCIP_EVENTHDLR* eventhdlr, SCIP_EVENT* event, SCIP_EVENTDATA* eventdata)
SCIP_DECL_EVENTEXEC(LPObserver::scip_exec)
{
  const SCIP_EVENTTYPE eventType = SCIPeventGetType(event);

  if(eventType == SCIP_EVENTTYPE_LPSOLVED)
  {
    idx rows = SCIPgetNLPRows(scip);
    idx cols = SCIPgetNLPCols(scip);

    ++numLPs;

    numRows += rows;
    numCols += cols;

    maxRows = std::max(maxRows, rows);
    maxCols = std::max(maxCols, cols);

    minRows = std::min(minRows, rows);
    minCols = std::min(minCols, cols);

    if(!rootSolved)
    {
      double val = SCIPgetLPRootObjval(scip);
      if(val != SCIP_INVALID)
      {
        rootSolved = true;
        LProotObjVal = val;
      }
    }
  }
  else if(eventType & SCIP_EVENTTYPE_NODESOLVED)
  {
    if(SCIPgetCurrentNode(scip) == SCIPgetRootNode(scip))
    {
      rootSolvingTime = rootTimer.elapsed();
    }
  }
  else if(eventType & SCIP_EVENTTYPE_NODEFOCUSED)
  {
    if(SCIPgetCurrentNode(scip) == SCIPgetRootNode(scip))
    {
      rootTimer.reset();
    }
  }

  return SCIP_OKAY;
}

SCIP_DECL_EVENTEXIT(LPObserver::scip_exit)
{
  if(!rootSolved)
  {
    double val = SCIPgetLPRootObjval(scip);
    if(val != SCIP_INVALID)
    {
      rootSolved = true;
      LProotObjVal = val;
    }
  }

  return SCIP_OKAY;
}

SCIP_DECL_EVENTEXITSOL(LPObserver::scip_exitsol)
{
  SCIP_CALL(SCIPdropEvent(scip, SCIP_EVENTTYPE_LPSOLVED, eventhdlr, NULL, -1) );

  SCIP_CALL(SCIPdropEvent(scip, SCIP_EVENTTYPE_NODESOLVED, eventhdlr, NULL, -1) );

  SCIP_CALL_EXC(SCIPcatchEvent(scip, SCIP_EVENTTYPE_NODEFOCUSED, eventhdlr, NULL, NULL));

  return SCIP_OKAY;
}

LPStats LPObserver::getStats() const
{
  return LPStats{getNumLPs(),
      getAvgRows(),
      getAvgCols(),
      getMaxRows(),
      getMaxCols(),
      getMinRows(),
      getMinCols(),
      getLProotObjVal(),
      rootLPSolved(),
      rootNodeSolvingTime()
      };
}
