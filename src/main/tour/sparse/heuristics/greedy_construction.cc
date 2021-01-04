#include "greedy_construction.hh"

#include "tour/sparse/pricers/sparse_pricer.hh"

#define NAME "greedy_construction"

GreedyConstruction::GreedyConstruction(SparseProgram& program,
                                       const std::string& name,
                                       const std::string& description,
                                       char symbol,
                                       int numRuns)
  : scip::ObjHeur(program.getSCIP(),            // SCIP
                  name.c_str(),                 // name
                  description.c_str(),          // description
                  symbol,                       // display character
                  10000,                        // priority
                  1,                            // frequency
                  0,                            // frequency offset
                  -1,                           // max depth
                  SCIP_HEURTIMING_AFTERLPNODE,  // timing
                  FALSE),                       // subscip
  program(program),
  graph(program.getGraph()),
  originalGraph(graph.underlyingGraph()),
  source(program.getSource()),
  numRuns(numRuns),
  currentSeed(0)
{
  assert(numRuns >= 0);
}

TimedEdge GreedyConstruction::bestEdge(const TimedVertex& source,
                                       const std::vector<TimedEdge>& validEdges) const
{
  assert(!validEdges.empty());

  double bestScore = -inf;

  TimedEdge bestEdge = validEdges.front();

  for(const TimedEdge& currentEdge : validEdges)
  {
    double currentScore = score(currentEdge);

    if(currentScore > bestScore)
    {
      bestEdge = currentEdge;
      bestScore = currentScore;
    }
  }

  return bestEdge;

}

TimedEdge GreedyConstruction::selectEdge(const TimedVertex& source,
                                         const std::vector<TimedEdge>& validEdges,
                                         std::default_random_engine& engine) const
{
  assert(!validEdges.empty());

  double totalScore = 0;

  for(const TimedEdge& currentEdge : validEdges)
  {
    totalScore += score(currentEdge);
  }

  std::uniform_real_distribution<double> distribution(0., totalScore);

  double targetValue = distribution(engine);
  double currentValue = 0;

  for(const TimedEdge& currentEdge : validEdges)
  {
    assert(currentValue <= targetValue);

    double currentScore = score(currentEdge);

    if(currentValue + currentScore > targetValue)
    {
      return currentEdge;
    }

    currentValue += currentScore;
  }

  return validEdges.back();
}

std::vector<TimedEdge> GreedyConstruction::validEdges(const TimedVertex& current,
                                                      const VertexSet& visited,
                                                      const EdgeSet& forbidden) const
{
  std::vector<TimedEdge> edges;

  for(const TimedEdge& outgoing : graph.getOutgoing(current))
  {
    // don't visit again...
    if(visited.contains(graph.underlyingVertex(outgoing.getTarget())))
    {
      continue;
    }

    // don't walk back to the source
    if(graph.underlyingVertex(outgoing.getTarget()) == source)
    {
      continue;
    }

    // don't use forbidden edges...
    if(forbidden.contains(graph.underlyingEdge(outgoing)))
    {
      continue;
    }

    edges.push_back(outgoing);
  }

  return edges;
}

bool GreedyConstruction::constructBestTour(const EdgeSet& forbidden,
                                           TimedPath& timedPath)
{
  const double cutoff = program.upperBound();
  VertexSet visited(originalGraph);

  TimedVertex currentVertex = graph.getVertex(source, 0);

  const idx numVertices = originalGraph.getVertices().size();

  for(idx i = 0; i < numVertices - 1; ++i)
  {
    if(cmp::ge(currentVertex.getTime(), cutoff))
    {
      //Log(debug) << "Exceeded cutoff";
      return false;
    }

    std::vector<TimedEdge> nextEdges = validEdges(currentVertex,
                                                  visited,
                                                  forbidden);

    if(nextEdges.empty())
    {
      //Log(debug) << "Could not extend path";
      return false;
    }

    TimedEdge nextEdge = bestEdge(currentVertex, nextEdges);

    timedPath.append(nextEdge);
    currentVertex = nextEdge.getTarget();
    visited.insert(graph.underlyingVertex(currentVertex));
  }

  bool foundSource = false;

  {
    for(const TimedEdge& outgoing : graph.getOutgoing(currentVertex))
    {
      if(graph.underlyingVertex(outgoing.getTarget()) == source)
      {
        foundSource = true;
        timedPath.append(outgoing);
        currentVertex = outgoing.getTarget();
        break;
      }
    }
  }

  return foundSource;
}

bool GreedyConstruction::constructTour(const EdgeSet& forbidden,
                                       TimedPath& timedPath,
                                       std::default_random_engine& engine)
{
  const double cutoff = program.upperBound();
  VertexSet visited(originalGraph);

  TimedVertex currentVertex = graph.getVertex(source, 0);

  const idx numVertices = originalGraph.getVertices().size();

  for(idx i = 0; i < numVertices - 1; ++i)
  {
    if(cmp::ge(currentVertex.getTime(), cutoff))
    {
      // Log(debug) << "Exceeded cutoff";
      return false;
    }

    std::vector<TimedEdge> nextEdges = validEdges(currentVertex,
                                                  visited,
                                                  forbidden);

    if(nextEdges.empty())
    {
      // Log(debug) << "Could not extend path";
      return false;
    }

    TimedEdge nextEdge = selectEdge(currentVertex, nextEdges, engine);

    timedPath.append(nextEdge);
    currentVertex = nextEdge.getTarget();
    visited.insert(graph.underlyingVertex(currentVertex));
  }

  bool foundSource = false;

  {
    for(const TimedEdge& outgoing : graph.getOutgoing(currentVertex))
    {
      if(graph.underlyingVertex(outgoing.getTarget()) == source)
      {
        foundSource = true;
        timedPath.append(outgoing);
        currentVertex = outgoing.getTarget();
        break;
      }
    }
  }

  return foundSource;
}

SCIP_DECL_HEUREXEC(GreedyConstruction::scip_exec)
{
  int numImprovements = 0;
  int totalNumRuns = 0;

  *result = SCIP_DIDNOTFIND;

  if(!startRound())
  {
    return SCIP_OKAY;
  }

  EdgeSet forbidden = program.getForbiddenEdges();

  {
    TimedPath timedPath;

    if(constructBestTour(forbidden, timedPath))
    {
      if(addAsTour(timedPath, heur))
      {
        ++numImprovements;

        *result = SCIP_FOUNDSOL;
      }
    }
  }


  std::default_random_engine engine(currentSeed++);

  for(int currentRun = 0; currentRun < numRuns; ++currentRun, ++totalNumRuns)
  {
    const double cutoff = program.upperBound();

    TimedPath timedPath;

    if(!constructTour(forbidden, timedPath, engine))
    {
      continue;
    }

    num arrivalTime = timedPath.getTarget().getTime();

    if(cmp::ge(arrivalTime, cutoff))
    {
      continue;
    }

    if(addAsTour(timedPath, heur))
    {
      ++numImprovements;
      currentRun = 0;

      *result = SCIP_FOUNDSOL;
    }
  }

  Log(info) << "Found " << numImprovements
            << " improved tours during " << totalNumRuns
            << " runs";

  return SCIP_OKAY;
}

bool GreedyConstruction::addAsTour(const TimedPath& timedPath,
                                   SCIP_HEUR* heur)
{
  assert(timedPath.underlyingPath(graph).isTour(originalGraph));

  idx numVertices = originalGraph.getVertices().size();

  Log(info) << "Adding a tour with arrival time " << timedPath.getTarget().getTime();

  std::vector<Vertex> vertices;
  vertices.reserve(numVertices);

  for(const TimedEdge& timedEdge : timedPath.getEdges())
  {
    vertices.push_back(graph.underlyingVertex(timedEdge.getSource()));
  }

  assert(vertices.size() == numVertices);

  Tour tour(originalGraph, vertices);

  return program.getPricingManager().addTour(tour, heur);
}
