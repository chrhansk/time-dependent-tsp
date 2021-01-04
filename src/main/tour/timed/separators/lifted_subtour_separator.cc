#include "lifted_subtour_separator.hh"

#include <sstream>

#include "scip_utils.hh"

#include "log.hh"

#include "arborescence/min_arborescence.hh"
#include "flow/max_flow.hh"
#include "tour/static/tour_solver.hh"

#define NAME "Lifted subtour separator"

const double cutoff = 1e-6;

LiftedSubtourSeparator::LiftedSubtourSeparator(SimpleProgram& program,
                                               const DistanceFunc& distances,
                                               idx timeHorizon,
                                               bool subtourBound)
  : scip::ObjSepa(program.getSCIP(),
                  NAME,
                  "Separates lifted subtours",
                  100000,
                  0,
                  0.0,
                  TRUE,
                  FALSE),
  program(program),
  graph(program.getGraph()),
  distances(distances),
  timeHorizon(timeHorizon),
  subtourBound(subtourBound)
{
}

void LiftedSubtourSeparator::generateCut(SCIP* scip,
                                         SCIP_SEPA* sepa,
                                         const VertexSet& originalVertices,
                                         idx maxTime)
{
  std::ostringstream namebuf;
  SCIP_ROW* cut;

  const EdgeMap<SCIP_VAR*>& variables = program.getVariables();
  const TimeExpandedGraph& graph = program.getGraph();

  idx numOriginalVertices = 0;

  for(const Vertex& vertex : graph.underlyingGraph().getVertices())
  {
    if(originalVertices.contains(vertex))
    {
      ++numOriginalVertices;
    }
  }

  namebuf << "subtour_elimination_" << numOriginalVertices;

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),
                                       1,
                                       SCIPinfinity(scip),
                                       FALSE,
                                       FALSE,
                                       TRUE));

  idx numEdges = 0;

  for(const TimedEdge& edge : graph.getEdges())
  {
    if(edge.getSource().getTime() > maxTime)
    {
      continue;
    }

    if(graph.underlyingEdge(edge).leaves(originalVertices))
    {
      SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(edge), 1.));
      ++numEdges;
    }
  }

  assert(numEdges > 0);

  Log(info) << "Created a cut with " << numEdges << " edges";

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));

  /* release the row */
  SCIP_CALL_EXC(SCIPreleaseRow(scip, &cut));
}

/** reduced cost pricing method of variable pricer for feasible LPs */
SCIP_DECL_SEPAEXECLP(LiftedSubtourSeparator::scip_execlp)
{
  Log(info) << "Starting lifted subtour separation";

  Vertex source = program.getSource();

  const EdgeMap<SCIP_VAR*>& variables = program.getVariables();

  EdgeSolutionValues solutionValues(scip, variables.getValues());
  EdgeMap<double> combinedFlow = graph.combinedValues(solutionValues);

  MinCutResult cutResult = computeMinCut(program.getGraph().underlyingGraph(),
                                         combinedFlow.getValues(),
                                         source);

  if(cutResult.value >= 1 - cutoff)
  {
    Log(info) << "Could not find a cut";
  }
  else
  {
    Log(info) << "Found a cut with a value of " << cutResult.value;

    assert(cutResult.cut.contains(source));

    idx maxTime = computeMaxTime(cutResult.cut);

    Log(info) << "Cutoff time: " << timeHorizon - maxTime
              << " (time horizon: " << timeHorizon
              << ")";

    generateCut(scip, sepa, cutResult.cut, timeHorizon - maxTime);

    *result = SCIP_SEPARATED;
  }

  return SCIP_OKAY;
}

idx LiftedSubtourSeparator::computeMaxTime(const VertexSet& originalVertices)
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

std::string LiftedSubtourSeparator::getName() const
{
  return NAME;
}
