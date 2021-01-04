#ifndef SIMPLE_ODD_PATH_FREE_SEPARATOR_HH
#define SIMPLE_ODD_PATH_FREE_SEPARATOR_HH

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <objscip/objscip.h>

#include "tour/timed/simple_program.hh"

#include "tour/separators/odd_path_free_separator.hh"

class SimpleOddPathFreeSeparator : public scip::ObjSepa
{
private:
  SimpleProgram& program;
  const TimeExpandedGraph& graph;
  const Graph& originalGraph;
  const EdgeMap<SCIP_VAR*>& variables;

  OddPathFreeSeparator separator;

  void generateCut(SCIP* scip,
                   SCIP_SEPA* sepa,
                   const OddPathFreeSet& oddPathFreeSet);


public:
  SimpleOddPathFreeSeparator(SimpleProgram& program);

  virtual SCIP_DECL_SEPAEXECLP(scip_execlp) override;
};


#endif /* SIMPLE_ODD_PATH_FREE_SEPARATOR_HH */
