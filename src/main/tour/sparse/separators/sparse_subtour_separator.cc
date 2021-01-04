#include "sparse_subtour_separator.hh"

#include <sstream>

#include "flow/max_flow.hh"
#include "tour/sparse/pricers/sparse_pricer.hh"

SparseSubTourCut::SparseSubTourCut(SparseSeparator& separator,
                                   const VertexSet& vertices,
                                   SCIP_SEPA* sepa)
  : SparseCut(separator),
    graph(separator.getProgram().getGraph()),
    originalGraph(graph.underlyingGraph()),
    vertices(vertices)
{
  generateCut(sepa);
}


void SparseSubTourCut::generateCut(SCIP_SEPA* sepa)
{
  SCIP* scip = separator.getSCIP();

  std::ostringstream namebuf;

  {
    idx numOriginalVertices = 0;

    for(const Vertex& vertex : originalGraph.getVertices())
    {
      if(vertices.contains(vertex))
      {
        ++numOriginalVertices;
      }
    }

    namebuf << "subtour_elimination_" << numOriginalVertices;
  }

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),
                                       1,
                                       SCIPinfinity(scip),
                                       FALSE,
                                       TRUE,
                                       TRUE));

  for(const Edge& edge : originalGraph.getEdges())
  {
    if(!edge.leaves(vertices))
    {
      continue;
    }

    for(const TimedEdge& timedEdge : graph.getTimedEdges(edge))
    {
      SCIP_VAR* variable = variables(timedEdge);

      if(!variable)
      {
        continue;
      }

      SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variable, 1.));
    }
  }

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));
}

void SparseSubTourCut::addedEdge(const TimedEdge& timedEdge)
{
  SCIP* scip = separator.getSCIP();

  const Edge& edge = graph.underlyingEdge(timedEdge);

  if(edge.leaves(vertices))
  {
    SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(timedEdge), 1.));
  }
}

void SparseSubTourCut::addDualCosts(EdgeMap<double>& dualCosts,
                                    DualCostType costType) const
{
  const double costCoefficient = getDual(costType);

  for(const Edge& edge : originalGraph.getEdges())
  {
    if(!edge.leaves(vertices))
    {
      continue;
    }

    for(const TimedEdge& timedEdge : graph.getTimedEdges(edge))
    {
      dualCosts(timedEdge) += costCoefficient;
    }
  }
}


std::vector<std::unique_ptr<SparseCut>>
SparseSubtourSeparator::separate(const EdgeFunc<double>& values,
                                 SCIP_SEPA* sepa,
                                 int maxNumCuts)
{
  std::vector<std::unique_ptr<SparseCut>> cuts{};

  const Graph& originalGraph = graph.underlyingGraph();

  EdgeMap<double> flow = graph.combinedValues(program.solutionValues());

  MinCutResult cutResult = computeMinCut(originalGraph,
                                         flow.getValues());

  if(cmp::lt(cutResult.value, 1))
  {
    cuts.push_back(std::make_unique<SparseSubTourCut>(*this, cutResult.cut, sepa));

    Log(info) << "Found a violated min cut (value: " << cutResult.value << ")";
  }
  else
  {
    Log(info) << "Could not find a violated min cut";
  }

  return cuts;
}
