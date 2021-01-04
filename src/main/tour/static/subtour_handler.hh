#ifndef SUBTOUR_HANDLER_HH
#define SUBTOUR_HANDLER_HH

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <objscip/objscip.h>

#include "tour_solver.hh"

class SubtourHandler : public scip::ObjConshdlr
{
private:
  const Graph& graph;
  const EdgeMap<SCIP_VAR*>& variables;

  SCIP_ROW* generateSubtourConstraint(SCIP* scip,
                                      SCIP_CONSHDLR* conshdlr,
                                      const VertexSet& cut);

  bool separate(SCIP* scip,
                SCIP_CONSHDLR* conshdlr,
                SCIP_SOL* solution = NULL);

  bool findSubtour(SCIP* scip,
                   SCIP_SOL* solution);

public:
  SubtourHandler(SCIP* scip,
                 const Graph& graph,
                 const EdgeMap<SCIP_VAR*>& variables);

  virtual SCIP_DECL_CONSSEPALP(scip_sepalp) override;

  virtual SCIP_DECL_CONSSEPASOL(scip_sepasol) override;

  virtual SCIP_DECL_CONSHDLRISCLONEABLE(iscloneable) override
  {
    return false;
  }

  virtual SCIP_DECL_CONSLOCK(scip_lock) override;

  virtual SCIP_DECL_CONSCHECK(scip_check) override;

  virtual SCIP_DECL_CONSENFOPS(scip_enfops) override;

  virtual SCIP_DECL_CONSENFOLP(scip_enfolp) override;

  virtual SCIP_DECL_CONSTRANS(scip_trans) override;

  std::string getName() const;

};

#endif /* SUBTOUR_HANDLER_HH */
