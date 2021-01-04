#ifndef LIFTED_SUBTOUR_SEPARATOR_HH
#define LIFTED_SUBTOUR_SEPARATOR_HH

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <objscip/objscip.h>

#include "tour/timed/simple_program.hh"


class LiftedSubtourSeparator : public scip::ObjSepa
{
private:
  const SimpleProgram& program;
  const TimeExpandedGraph& graph;
  const DistanceFunc& distances;
  idx timeHorizon;
  bool subtourBound;

  virtual void generateCut(SCIP* scip,
                           SCIP_SEPA* sepa,
                           const VertexSet& originalVertices,
                           idx maxTime);

  idx computeMaxTime(const VertexSet& originalVertices);

public:
  LiftedSubtourSeparator(SimpleProgram& program,
                         const DistanceFunc& distances,
                         idx timeHorizon,
                         bool subTourBound = false);

  /** reduced cost pricing method of variable pricer for feasible LPs */
  virtual SCIP_DECL_SEPAEXECLP(scip_execlp) override;

  std::string getName() const;
};


#endif /* LIFTED_SUBTOUR_SEPARATOR_HH */
