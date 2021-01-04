#ifndef SPARSE_SOLUTION_VALUES_HH
#define SPARSE_SOLUTION_VALUES_HH

#include "graph/edge_map.hh"

class SparseSolutionValues : public EdgeFunc<double>
{
private:
  SCIP* scip;
  const EdgeFunc<SCIP_VAR*>& vars;

public:
  SparseSolutionValues(SCIP* scip, const EdgeFunc<SCIP_VAR*>& vars)
    : scip(scip), vars(vars)
  {}

  double operator()(const Edge& edge) const override
  {
    SCIP_VAR* var = vars(edge);

    if(var)
    {
      return SCIPgetVarSol(scip, var);
    }

    return 0;
  }
};


#endif /* SPARSE_SOLUTION_VALUES_HH */
