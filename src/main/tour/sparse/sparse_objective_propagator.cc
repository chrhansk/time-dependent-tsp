#include "sparse_objective_propagator.hh"

#include "scip_utils.hh"
#include "tour/sparse/pricers/sparse_pricing_manager.hh"

#define NAME "sparse_objective_propagator"

SparseObjectivePropagator::SparseObjectivePropagator(SparseProgram& program)
  : scip::ObjProp(program.getSCIP(),
                  NAME,
                  "Fixes edges based on objective value",
                  100000,                                                     // priority
                  1,                                                          // freq
                  FALSE,                                                      // delay
                  SCIP_PROPTIMING_AFTERLPLOOP,                                // timingmask
                  -1,                                                         // presolpriority
                  1,                                                          // presolmaxrounds
                  SCIP_PRESOLTIMING_FAST                                      // presoltiming
    ),
  program(program),
  graph(program.getGraph()),
  variables(program.getPricingManager().getVariables()),
  cutoffBound(program.getTimeHorizon())
{
}

idx SparseObjectivePropagator::propagatePrimalBound(double upperBound)
{
  SCIP* scip = program.getSCIP();

  idx numFixed = 0;

  if(upperBound >= cutoffBound)
  {
    return 0;
  }

  cutoffBound = upperBound;

  for(const TimedEdge& timedEdge : graph.getEdges())
  {
    SCIP_VAR* var = variables(timedEdge);

    if(!var)
    {
      continue;
    }

    // disregard already fixed variables
    if(SCIPvarGetLbLocal(var) > 0.5 || SCIPvarGetUbLocal(var) < 0.5)
    {
      continue;
    }

    if(cmp::ge(timedEdge.getTarget().getTime(), cutoffBound))
    {
      SCIP_CALL_EXC(SCIPchgVarUb(scip, var, 0.0));

      ++numFixed;
    }
  }


  return numFixed;
}

idx SparseObjectivePropagator::propagateDualBound(double lowerBound) const
{
  const Vertex source = program.getSource();

  SCIP* scip = program.getSCIP();

  idx numFixed = 0;

  for(const TimedVertex& timedTarget : graph.getExpandedVertices(source))
  {
    if(cmp::ge(timedTarget.getTime(), lowerBound))
    {
      continue;
    }

    for(const TimedEdge& incoming : graph.getIncoming(timedTarget))
    {
      SCIP_VAR* var = variables(incoming);

      if(!var)
      {
        continue;
      }

      // disregard already fixed variables
      if(SCIPvarGetLbLocal(var) > 0.5 || SCIPvarGetUbLocal(var) < 0.5)
      {
        continue;
      }

      /*
        Log(info) << "Fixing edge "
        << graph.underlyingVertex(incoming.getSource())
        << "_"
        << graph.underlyingVertex(incoming.getTarget())
        << "@"
        << incoming.getTarget().getTime();
      */


      SCIP_CALL_EXC(SCIPchgVarUb(scip, var, 0.0));

      ++numFixed;
    }

  }

  return numFixed;
}


SCIP_DECL_PROPEXEC(SparseObjectivePropagator::scip_exec)
{
  idx numDual = 0, numPrimal = 0;

  const double lowerBound = program.lowerBound();
  const double upperBound = program.upperBound();

  numDual = propagateDualBound(lowerBound);
  numPrimal = propagatePrimalBound(upperBound);

  idx numFixed = numDual + numPrimal;

  if(numDual > 0)
  {
    Log(info) << "Propagator fixed " << numDual << " edges (using lower bound = " << lowerBound << ")";
  }

  if(numPrimal > 0)
  {
    Log(info) << "Propagator fixed " << numPrimal << " edges (using upper bound = " << upperBound << ")";
  }

  *result = (numFixed > 0) ? SCIP_REDUCEDDOM : SCIP_DIDNOTFIND;

  return SCIP_OKAY;
}
