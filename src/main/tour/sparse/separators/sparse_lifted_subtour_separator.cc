#include "sparse_lifted_subtour_separator.hh"

#include <sstream>

#include "arborescence/min_arborescence.hh"
#include "flow/max_flow.hh"
#include "tour/static/tour_solver.hh"

SparseLiftedSubtourCut::SparseLiftedSubtourCut(SparseSeparator& separator,
                                               const VertexSet& originalVertices,
                                               idx maxTime,
                                               SCIP_SEPA* sepa)
  : SparseCut(separator),
    originalVertices(originalVertices),
    maxTime(maxTime)
{
  createCut(sepa);
}


void SparseLiftedSubtourCut::createCut(SCIP_SEPA* sepa)
{
  std::ostringstream namebuf;

  const TimeExpandedGraph& graph = separator.getProgram().getGraph();
  const Graph& originalGraph = graph.underlyingGraph();

  idx numOriginalVertices = 0;

  for(const Vertex& originalVertex : originalGraph.getVertices())
  {
    if(originalVertices.contains(originalVertex))
    {
      ++numOriginalVertices;
    }
  }

  namebuf << "subtour_elimination_" << numOriginalVertices;

  SCIP* scip = separator.getSCIP();

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),
                                       1,
                                       SCIPinfinity(scip),
                                       FALSE,  // local
                                       TRUE,   // modifiable
                                       TRUE)); // removable

  for(const Vertex& originalVertex : originalGraph.getVertices())
  {
    if(!originalVertices.contains(originalVertex))
    {
      continue;
    }

    for(const Edge& originalEdge : originalGraph.getOutgoing(originalVertex))
    {
      if(originalEdge.leaves(originalVertices))
      {
        for(const TimedEdge& timedEdge : graph.getTimedEdges(originalEdge))
        {
          if(timedEdge.getSource().getTime() <= maxTime)
          {
            SCIP_VAR* var = variables(timedEdge);

            if(var)
            {
              SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, var, 1.));
            }
          }
        }
      }
    }
  }

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));
}

void SparseLiftedSubtourCut::addedEdge(const TimedEdge& timedEdge)
{
  SCIP* scip = separator.getSCIP();
  const TimeExpandedGraph& graph = separator.getProgram().getGraph();

  const Edge& edge = graph.underlyingEdge(timedEdge);

  if(edge.leaves(originalVertices) &&
     timedEdge.getSource().getTime() <= maxTime)
  {
    SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(timedEdge), 1.));
  }
}

void SparseLiftedSubtourCut::addDualCosts(EdgeMap<double>& dualCosts, DualCostType costType) const
{
  const double costCoefficient = getDual(costType);

  const TimeExpandedGraph& graph = separator.getProgram().getGraph();
  const Graph& originalGraph = graph.underlyingGraph();

  for(const Vertex& originalVertex : originalGraph.getVertices())
  {
    if(!originalVertices.contains(originalVertex))
    {
      continue;
    }

    for(const Edge& originalEdge : originalGraph.getOutgoing(originalVertex))
    {
      if(originalEdge.leaves(originalVertices))
      {
        for(const TimedEdge& timedEdge : graph.getTimedEdges(originalEdge))
        {
          if(timedEdge.getSource().getTime() <= maxTime)
          {
            dualCosts(timedEdge) += costCoefficient;
          }
        }
      }
    }
  }
}


idx SparseLiftedSubtourSeparator::computeMaxTime(const VertexSet& originalVertices)
{
  const Graph& graph = program.getGraph().underlyingGraph();

  Vertex originalSource = program.getInitialTour().getVertices().front();

  std::unordered_map<Vertex, Vertex> vertices, invVertices;

  Graph subGraph;

  auto isValidVertex = [&](const Vertex& vertex) -> bool
    {
      return vertex != originalSource && !originalVertices.contains(vertex);
    };

  std::vector<Vertex> cutVertices;

  for(const Vertex& vertex : graph.getVertices())
  {
    if(originalVertices.contains(vertex))
    {
      cutVertices.push_back(vertex);
    }
  }

  for(const Vertex& vertex : graph.getVertices())
  {
    if(!isValidVertex(vertex))
    {
      continue;
    }

    Vertex subVertex = subGraph.addVertex();
    vertices.insert(std::make_pair(subVertex, vertex));
    invVertices.insert(std::make_pair(vertex, subVertex));
  }

  Vertex source = subGraph.addVertex();
  Vertex target = subGraph.addVertex();

  for(const Edge& edge : graph.getEdges())
  {
    if(isValidVertex(edge.getSource()) &&
       isValidVertex(edge.getTarget()))
    {
      subGraph.addEdge(invVertices[edge.getSource()],
                       invVertices[edge.getTarget()]);
    }
  }

  for(const Vertex& vertex : graph.getVertices())
  {
    if(!isValidVertex(vertex))
    {
      continue;
    }

    subGraph.addEdge(source, invVertices[vertex]);
    subGraph.addEdge(invVertices[vertex], target);
  }

  subGraph.addEdge(target, source);

  auto subDistances = SimpleDistances([&](const Vertex& first, const Vertex& second) -> num
                                      {
                                        if(first == target)
                                        {
                                          assert(second == source);

                                          return 0;
                                        }

                                        if(first == source)
                                        {
                                          assert(second != target);

                                          num value = inf;

                                          for(const Vertex& vertex : cutVertices)
                                          {
                                            value = std::min(value, distances(vertex, second));
                                          }

                                          return value;
                                        }
                                        if(second == target)
                                        {
                                          return distances(first, originalSource);
                                        }

                                        return distances(first, second);
                                      });

  if(subtourBound)
  {
    TourSolver subSolver(subGraph,
                         subDistances,
                         Program::Settings().withSolverOutput(false));

    Tour tour = subSolver.findTour();

    return tour.cost(subDistances);
  }
  else
  {
    EdgeMap<num> subWeights(subGraph, 0);

    for(const Edge& subEdge : subGraph.getEdges())
    {
      subWeights(subEdge) = subDistances(subEdge.getSource(), subEdge.getTarget());
    }

    EdgeSet subArborescence = minArborescence(subGraph,
                                              source,
                                              subWeights.getValues());

    double totalCost = 0;

    for(const Edge& subEdge : subGraph.getEdges())
    {
      if(subArborescence.contains(subEdge))
      {
        totalCost += subWeights(subEdge);
      }
    }

    return totalCost;
  }
}

std::vector<std::unique_ptr<SparseCut>>
SparseLiftedSubtourSeparator::separate(const EdgeFunc<double>& values,
                                       SCIP_SEPA* sepa,
                                       int maxNumCuts)
{
  std::vector<std::unique_ptr<SparseCut>> cuts{};

  const TimeExpandedGraph& graph = getProgram().getGraph();
  const Graph& originalGraph = graph.underlyingGraph();

  EdgeMap<double> flow = graph.combinedValues(program.solutionValues());

  Vertex source = program.getSource();

  MinCutResult cutResult = computeMinCut(originalGraph,
                                         flow.getValues(),
                                         source);

  if(cmp::lt(cutResult.value, 1))
  {
    Log(info) << "Found a violated min cut (value: " << cutResult.value << ")";

    idx timeHorizon = program.getTimeHorizon();
    idx maxTime = computeMaxTime(cutResult.cut);

    Log(info) << "Cutoff time: " << timeHorizon - maxTime
              << " (time horizon: " << timeHorizon
              << ")";

    cuts.push_back(std::make_unique<SparseLiftedSubtourCut>(*this,
                                                            cutResult.cut,
                                                            timeHorizon - maxTime,
                                                            sepa));
  }
  else
  {
    Log(info) << "Could not find a violated min cut";
  }

  return cuts;
}
