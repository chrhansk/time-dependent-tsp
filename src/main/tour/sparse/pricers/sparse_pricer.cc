#include "sparse_pricer.hh"

#include "tour/sparse/sparse_program.hh"

#include "sparse_pricing_manager.hh"

SparsePricer::SparsePricer(SparseProgram& program)
  : program(program),
    manager(program.getPricingManager()),
    scip(program.getSCIP()),
    graph(program.getGraph()),
    source(program.getSource())
{}

const VertexMap<SCIP_CONS*>& SparsePricer::getFlowConstraints() const
{
  return manager.getFlowConstraints();
}

const EdgeMap<SCIP_CONS*>& SparsePricer::getLinkingConstraints() const
{
  return manager.getLinkingConstraints();
}

const VertexMap<SCIP_CONS*>& SparsePricer::getCoveringConstraints() const
{
  return manager.getCoveringConstraints();
}

const EdgeMap<SCIP_VAR*>& SparsePricer::getVariables() const
{
  return manager.getVariables();
}
