#ifndef SPARSE_CUT_HH
#define SPARSE_CUT_HH

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include "scip_utils.hh"

#include "tour/sparse/sparse_program.hh"

class SparseSeparator;

class SparseCut
{
protected:
  SparseSeparator& separator;
  const EdgeMap<SCIP_VAR*>& variables;
  SCIP_ROW* cut;

public:
  SparseCut(SparseSeparator& separator);

  SparseCut(const SparseCut&) = delete;
  SparseCut& operator=(const SparseCut&) = delete;

  double getDual(DualCostType costType) const;

  virtual void addedEdge(const TimedEdge& timedEdge) = 0;

  virtual void addDualCosts(EdgeMap<double>& dualCosts,
                            DualCostType costType) const = 0;

  virtual ~SparseCut();
};


#endif /* SPARSE_CUT_HH */
