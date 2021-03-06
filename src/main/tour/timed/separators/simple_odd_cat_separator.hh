#ifndef SIMPLE_ODD_CAT_SEPARATOR_HH
#define SIMPLE_ODD_CAT_SEPARATOR_HH

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <objscip/objscip.h>

#include "tour/timed/simple_program.hh"

#include "tour/separators/odd_cat_separator.hh"

class SimpleOddCATSeparator : public scip::ObjSepa
{
private:
  SimpleProgram& program;
  const TimeExpandedGraph& graph;
  const Graph& originalGraph;
  OddCATSeparator separator;

  void generateCut(SCIP* scip,
                   SCIP_SEPA* sepa,
                   const std::vector<Edge>& originalCycle);

  void addEdgeToCut(SCIP_ROW* cut,
                    const Edge& edge,
                    double factor = 1);

public:
  SimpleOddCATSeparator(SimpleProgram& program);

  virtual SCIP_DECL_SEPAEXECLP(scip_execlp) override;
};

#endif /* SIMPLE_ODD_CAT_SEPARATOR_HH */
