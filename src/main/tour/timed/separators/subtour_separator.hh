#ifndef SUBTOUR_SEPARATOR_HH
#define SUBTOUR_SEPARATOR_HH

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <objscip/objscip.h>

#include "tour/timed/simple_program.hh"

class SubtourSeparator : public scip::ObjSepa
{
private:
  SimpleProgram& program;
  const TimeExpandedGraph& graph;

  virtual void generateCut(SCIP* scip,
                           SCIP_SEPA* sepa,
                           const VertexSet& originalVertices);

public:
  SubtourSeparator(SimpleProgram& program);

  /** reduced cost pricing method of variable pricer for feasible LPs */
  virtual SCIP_DECL_SEPAEXECLP(scip_execlp) override;

  std::string getName() const;
};


#endif /* SUBTOUR_SEPARATOR_HH */
