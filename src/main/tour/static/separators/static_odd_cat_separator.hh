#ifndef STATIC_ODD_CAT_SEPARATOR_HH
#define STATIC_ODD_CAT_SEPARATOR_HH

#include "tour/static/tour_solver.hh"
#include "tour/separators/odd_cat_separator.hh"

class StaticOddCATSeparator : public scip::ObjSepa
{
private:
  TourSolver& solver;
  const Graph& graph;
  OddCATSeparator separator;

  void generateCut(SCIP* scip,
                   SCIP_SEPA* sepa,
                   const std::vector<Edge>& cycle);

public:
  StaticOddCATSeparator(TourSolver& solver);

  virtual SCIP_DECL_SEPAEXECLP(scip_execlp) override;
};


#endif /* STATIC_ODD_CAT_SEPARATOR_HH */
