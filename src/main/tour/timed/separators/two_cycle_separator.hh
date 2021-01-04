#ifndef TWO_CYCLE_PRICER_HH
#define TWO_CYCLE_PRICER_HH

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <objscip/objscip.h>

#include "tour/timed/simple_program.hh"

class TwoCycleSeparator : public scip::ObjSepa
{
private:
  SimpleProgram& program;

  void generateCut(SCIP* scip,
                   SCIP_SEPA* sepa,
                   const TimedEdge& incoming);

public:
  TwoCycleSeparator(SimpleProgram& program);

  /** reduced cost pricing method of variable pricer for feasible LPs */
  virtual SCIP_DECL_SEPAEXECLP(scip_execlp);

  std::string getName() const;

};


#endif /* TWO_CYCLE_PRICER_HH */
