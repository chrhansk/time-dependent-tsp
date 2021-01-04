#ifndef SIMPLE_UNITARY_AFC_SEPARATOR_HH
#define SIMPLE_UNITARY_AFC_SEPARATOR_HH

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <objscip/objscip.h>

#include "tour/timed/simple_program.hh"
#include "tour/separators/unitary_afc_separator.hh"

class SimpleUnitaryAFCSeparator : public scip::ObjSepa
{
private:
  SimpleProgram& program;
  UnitaryAFCSeparator separator;

  void generateCut(SCIP* scip,
                   SCIP_SEPA* sepa,
                   const UnitaryAFCSet& set);

public:
  SimpleUnitaryAFCSeparator(SimpleProgram& program);

  /** reduced cost pricing method of variable pricer for feasible LPs */
  virtual SCIP_DECL_SEPAEXECLP(scip_execlp);

  std::string getName() const;
};


#endif /* SIMPLE_UNITARY_AFC_SEPARATOR_HH */
