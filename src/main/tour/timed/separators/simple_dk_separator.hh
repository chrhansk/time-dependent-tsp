#ifndef SIMPLE_DK_SEPARATOR_HH
#define SIMPLE_DK_SEPARATOR_HH

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <objscip/objscip.h>

#include "tour/timed/simple_program.hh"

#include "tour/separators/dk_separator.hh"

class SimpleDKSeparator : public scip::ObjSepa
{
private:
  SimpleProgram& program;
  const TimeExpandedGraph& graph;
  const Graph& originalGraph;
  DKSeparator separator;

  void generateCut(SCIP* scip,
                   SCIP_SEPA* sepa,
                   const std::vector<Vertex>& originalVertices);

  void addEdgeToCut(SCIP_ROW* cut,
                    const Edge& edge,
                    double factor = 1);

public:
  SimpleDKSeparator(SimpleProgram& program);

  virtual SCIP_DECL_SEPAEXECLP(scip_execlp) override;
};


#endif /* SIMPLE_DK_SEPARATOR_HH */
