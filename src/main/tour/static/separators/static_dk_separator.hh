#ifndef STATIC_DK_SEPARATOR_HH
#define STATIC_DK_SEPARATOR_HH

#include "tour/static/tour_solver.hh"
#include "tour/separators/dk_separator.hh"

class StaticDKSeparator : public scip::ObjSepa
{
private:
  TourSolver& solver;
  const Graph& graph;
  DKSeparator separator;

  void generateCut(SCIP* scip,
                   SCIP_SEPA* sepa,
                   const std::vector<Vertex>& vertices);

public:
  StaticDKSeparator(TourSolver& solver);

  virtual SCIP_DECL_SEPAEXECLP(scip_execlp) override;
};


#endif /* STATIC_DK_SEPARATOR_HH */
