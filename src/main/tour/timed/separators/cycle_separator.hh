#ifndef CYCLE_SEPARATOR_HH
#define CYCLE_SEPARATOR_HH

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <objscip/objscip.h>

#include "tour/timed/simple_program.hh"

class CycleSeparator : public scip::ObjSepa
{
private:
  SimpleProgram& program;
  const TimeExpandedGraph& graph;
  const Graph& originalGraph;

  bool generateCut(SCIP* scip,
                   SCIP_SEPA* sepa,
                   const TimedEdge& incoming,
                   const TimedPath& cycle);

  bool hasCycle(const TimedPath& path,
                TimedEdge& incoming,
                TimedPath& cycle);

public:
  CycleSeparator(SimpleProgram& program);

  /** reduced cost pricing method of variable pricer for feasible LPs */
  virtual SCIP_DECL_SEPAEXECLP(scip_execlp);

  std::string getName() const;
};


#endif /* CYCLE_SEPARATOR_HH */
