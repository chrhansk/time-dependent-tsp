#include "covering_relaxation.hh"


CoveringRelaxation::CoveringRelaxation(const TimeExpandedGraph& graph,
                                       Vertex source)
  : graph(graph),
    originalGraph(graph.underlyingGraph()),
    timedVertices(graph.getVertices().collect()),
    source(source),
    penalties(originalGraph, 0)
{
  std::sort(std::begin(timedVertices), std::end(timedVertices),
            [](const TimedVertex& first, const TimedVertex& second) -> bool
            {
              return first.getTime() < second.getTime();
            });
}

bool CoveringRelaxation::convertPath(const TimedPath& path,
                                     TimedPath& convertedPath) const
{
  VertexSet visited(originalGraph);

  visited.insert(source);

  {
    TimedVertex current = path.getSource();

    for(const TimedEdge& timedEdge : path.getEdges())
    {
      Vertex next = graph.underlyingVertex(timedEdge.getTarget());

      if(visited.contains(next))
      {
        continue;
      }

      bool found = false;

      for(const TimedEdge& outgoing : graph.getOutgoing(current))
      {
        if(graph.underlyingVertex(outgoing.getTarget()) == next)
        {
          visited.insert(next);
          convertedPath.append(outgoing);
          current = outgoing.getTarget();
          found = true;
          break;
        }
      }

      assert(found);
    }
  }

  VertexSet missed(originalGraph);
  idx numMissed = 0;

  for(const Vertex& vertex : originalGraph.getVertices())
  {
    if(!visited.contains(vertex))
    {
      missed.insert(vertex);
      ++numMissed;
    }
  }

  TimedVertex current = convertedPath.getTarget();

  for(idx i = 0; i < numMissed; ++i)
  {
    TimedEdge minEdge;

    bool found = false;
    idx minTime = inf;

    for(const TimedEdge& outgoing : graph.getOutgoing(current))
    {
      if(missed.contains(graph.underlyingVertex(outgoing.getTarget())))
      {
        idx travelTime = outgoing.travelTime();

        if(travelTime < minTime)
        {
          minTime = travelTime;
          minEdge = outgoing;
          found = true;
        }
      }
    }

    if(!found)
    {
      return false;
    }

    current = minEdge.getTarget();
    convertedPath.append(minEdge);

    missed.remove(graph.underlyingVertex(minEdge.getTarget()));
  }

  current = convertedPath.getTarget();

  bool found = false;

  for(const TimedEdge& outgoing : graph.getOutgoing(current))
  {
    if(graph.underlyingVertex(outgoing.getTarget()) == source)
    {
      convertedPath.append(outgoing);
      found = true;
    }
  }

  assert(found);

  assert(createTour(convertedPath).connects(originalGraph.getVertices().collect()));

  return true;
}

TimedPath CoveringRelaxation::findPath() const
{
  struct Label
  {
    Label() : cost(std::numeric_limits<double>::max()) {}

    Label(double cost,
          const TimedVertex& timedVertex)
      : cost(cost),
        timedVertex(timedVertex)
    {}

    Label(double cost,
          const TimedEdge& timedEdge)
      : cost(cost),
        timedEdge(timedEdge),
        timedVertex(timedEdge.getTarget())
    {}

    double cost;
    TimedEdge timedEdge;
    TimedVertex timedVertex;
  };

  VertexMap<Label> labels(graph, Label());

  TimedVertex timedSource = graph.getVertex(source, 0);

  Label sourceLabel(0, timedSource);

  labels(timedSource) = sourceLabel;

  TimedPath bestPath;
  double bestValue = std::numeric_limits<double>::max();

  for(const TimedVertex& timedVertex : timedVertices)
  {
    Label currentLabel = labels(timedVertex);

    if(graph.underlyingVertex(timedVertex) == source)
    {
      if(timedVertex.getTime() > 0)
      {
        if(currentLabel.cost < bestValue)
        {
          Label label = currentLabel;
          TimedPath path{};

          while(label.timedVertex != timedSource)
          {
            path.prepend(label.timedEdge);
            label = labels(label.timedEdge.getSource());
          }

          bestPath = path;
          bestValue = currentLabel.cost;
        }
      }
    }

    for(const TimedEdge& outgoing : graph.getOutgoing(timedVertex))
    {
      Label& nextLabel = labels(outgoing.getTarget());

      double edgeCosts = outgoing.travelTime() - penalties(graph.underlyingVertex(outgoing.getSource()));

      if(nextLabel.cost > currentLabel.cost + edgeCosts)
      {
        nextLabel.cost = currentLabel.cost + edgeCosts;
        nextLabel.timedEdge = outgoing;
      }
    }
  }

  return bestPath;
}

double CoveringRelaxation::evaluate(const TimedPath& path) const
{
  double value = path.travelTime();

  VertexMap<idx> counts(originalGraph, 0);

  for(const TimedEdge& timedEdge : path.getEdges())
  {
    ++counts(graph.underlyingVertex(timedEdge.getSource()));
  }

  for(const Vertex& vertex : originalGraph.getVertices())
  {
    value += (1 - ((double) counts(vertex))) * penalties(vertex);
  }

  return value;
}

Tour CoveringRelaxation::createTour(const TimedPath& path) const
{
  std::vector<Vertex> vertices;

  for(const TimedEdge& timedEdge : path.getEdges())
  {
    vertices.push_back(graph.underlyingVertex(timedEdge.getSource()));
  }

  return Tour(originalGraph, vertices);
}

RelaxationResult CoveringRelaxation::solve(idx maxIterations)
{
  double bestBound = 0;
  TimedPath bestPath;

  //TimedDistanceEvaluator timedEvaluator(distances);

  TimedPath bestTour;
  num bestValue = inf;

  num lastBound = -1;

  for(idx iteration = 1; iteration <= maxIterations; ++iteration)
  {
    TimedPath path = findPath();

    double bound = evaluate(path);

    {
      VertexMap<idx> counts(graph, 0);

      for(const Edge& edge : path.getEdges())
      {
        ++counts(edge.getSource());
      }

      bool isTour = true;
      bool coversAll = true;

      for(const Vertex& vertex : graph.getVertices())
      {
        if(counts(vertex) != 1)
        {
          isTour = false;
        }

        if(counts(vertex) == 0)
        {
          coversAll = false;
        }

        /*
        Log(info) << "Vertex "
                  << vertex.getIndex()
                  << " touches: "
                  << counts(vertex)
                  << " penalty: "
                  << penalties(vertex);
        */

      }

      if(isTour)
      {
        Log(info) << "Found the optimal tour";
      }
      if(coversAll)
      {
        Log(info) << "Covers all vertices!";
      }

    }

    VertexMap<int> subGradient(graph, 1);

    for(const Edge& edge : path.getEdges())
    {
      --subGradient(edge.getSource());
    }

    double stepSize = 5./((double) iteration);

    for(const Vertex& vertex : originalGraph.getVertices())
    {
      penalties(vertex) = penalties(vertex) + stepSize * subGradient(vertex);
    }

    {
      TimedPath convertedPath;

      if(convertPath(path, convertedPath))
      {
        num nextValue = convertedPath.travelTime();

        if(nextValue < bestValue)
        {
          Log(info) << "Primal bound improved: " << nextValue;
          bestTour = convertedPath;
          bestValue = nextValue;
        }
      }
    }

    if(bestBound < bound)
    {
      Log(info) << "Dual bound improved: " << bound;
      bestBound = bound;
      bestPath = path;
    }

    if(fabs(lastBound - bound) <= 1e-3)
    {
      break;
    }

    lastBound = bound;
  }

  Log(info) << "Best bound: " << bestBound;

  if(bestValue != inf)
  {
    Log(info) << "Best tour length: " << bestValue;
    return {bestBound, createTour(bestPath)};
  }
  else
  {
    return {bestBound, Tour(originalGraph, originalGraph.getVertices().collect())};
  }

}
