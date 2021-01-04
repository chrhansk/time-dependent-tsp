#include "sparse_unitary_afc_separator.hh"

#include <sstream>

#include "flow/max_flow.hh"

SparseUnitaryAFCCut::SparseUnitaryAFCCut(SparseSeparator& separator,
                                         const TimeExpandedGraph& graph,
                                         TimedEdge incoming,
                                         VertexSet vertices,
                                         SCIP_SEPA* sepa)
  : SparseCut(separator),
    graph(graph),
    incoming(incoming),
    vertices(vertices),
    originalSource(graph.underlyingVertex(incoming.getSource())),
    originalTarget(graph.underlyingVertex(incoming.getTarget()))
{
  createCut(sepa);
}

void SparseUnitaryAFCCut::createCut(SCIP_SEPA* sepa)
{
  std::ostringstream namebuf;

  namebuf << "unitary_afc_"
          << originalSource.getIndex()
          << "#"
          << incoming.getSource().getTime()
          << "_"
          << originalTarget.getIndex()
          << "#"
          << incoming.getTarget().getTime();

  SCIP* scip = separator.getSCIP();

  assert(variables(incoming));

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),
                                       0,
                                       SCIPinfinity(scip),
                                       FALSE,  // local
                                       TRUE,   // modifiable
                                       TRUE)); // removable

  SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(incoming), -1.));

  for(const TimedEdge& edge : graph.getEdges())
  {
    if(variables(edge) && validEdge(edge))
    {
      SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(edge), 1.));
    }
  }

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));
}

void SparseUnitaryAFCCut::addedEdge(const TimedEdge& edge)
{
  assert(variables(edge));

  if(!validEdge(edge))
  {
    return;
  }

  SCIP* scip = separator.getSCIP();

  SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(edge), 1.));
}

bool SparseUnitaryAFCCut::validEdge(const TimedEdge& edge) const
{
  if(!edge.leaves(vertices))
  {
    return false;
  }

  const Vertex& target = graph.underlyingVertex(edge.getTarget());

  if(target == originalSource || target == originalTarget)
  {
    return false;
  }

  return true;
}

void SparseUnitaryAFCCut::addDualCosts(EdgeMap<double>& dualCosts,
                                       DualCostType costType) const
{
  const double costCoefficient = getDual(costType);

  dualCosts(incoming) += (-costCoefficient);

  for(const TimedEdge& outgoing : graph.getEdges())
  {
    if(validEdge(outgoing))
    {
      dualCosts(outgoing) += costCoefficient;
    }
  }
}

std::vector<std::unique_ptr<SparseCut>>
SparseUnitaryAFCSeparator::separate(const EdgeFunc<double>& values,
                                    SCIP_SEPA* sepa,
                                    int maxNumSets)
{

  std::vector<std::unique_ptr<SparseCut>> cuts;

  std::vector<UnitaryAFCSet> sets = separator.separate(values, maxNumSets);

  for(const UnitaryAFCSet& set : sets)
  {
    cuts.push_back(std::make_unique<SparseUnitaryAFCCut>(*this,
                                                         graph,
                                                         set.getIncoming(),
                                                         set.getCut(),
                                                         sepa));
  }

  return cuts;
}
