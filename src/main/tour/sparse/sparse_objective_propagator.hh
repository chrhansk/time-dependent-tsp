#ifndef SPARSE_OBJECTIVE_PROPAGATOR_HH
#define SPARSE_OBJECTIVE_PROPAGATOR_HH

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <objscip/objscip.h>

#include "tour/sparse/sparse_program.hh"

class SparseObjectivePropagator : public scip::ObjProp
{
private:
  SparseProgram& program;
  const TimeExpandedGraph& graph;
  const EdgeMap<SCIP_VAR*>& variables;
  double cutoffBound;

  idx propagatePrimalBound(double nextCutoffBound);
  idx propagateDualBound(double lowerBound) const;

public:
  SparseObjectivePropagator(SparseProgram& program);

  SCIP_DECL_PROPEXEC(scip_exec);
};


#endif /* SPARSE_OBJECTIVE_PROPAGATOR_HH */
