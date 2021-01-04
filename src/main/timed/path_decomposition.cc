#include "path_decomposition.hh"

#include <sstream>

#include "log.hh"

std::vector<TimedPath> pathDecomposition(const TimeExpandedGraph& graph,
                                         const EdgeFunc<double>& flow,
                                         const Vertex& source)
{
  std::vector<TimedPath> paths;

  //EdgeSolutionValues solutionValues(scip, variables.getValues());

  EdgeMap<double> currentFlow(graph, flow);

  for(const TimedEdge& edge : graph.getEdges())
  {
    currentFlow(edge) = std::max(currentFlow(edge), 0.);
  }

  struct Entry
  {
    double value;
    TimedEdge edge;

    Entry()
      : value(-1)
    {}

    Entry(double value)
      : value(value)
    {}

    Entry(double value,
          const TimedEdge& edge)
      : value(value),
        edge(edge)
    {}

    bool isEmpty()
    {
      return value == -1;
    }

    static Entry empty()
    {
      return Entry(-1);
    }

  };

  const TimedVertex timedSource = graph.getVertex(source, 0);

  double remainingValue = 1.;

  while(cmp::pos(remainingValue))
  {
    VertexMap<Entry> entries(graph, Entry());

    bool found = false;
    TimedVertex timedTarget;

    entries(source) = Entry(remainingValue);

    for(const TimedVertex& currentVertex : graph.getTopologicalOrdering())
    {
      Entry currentEntry = entries(currentVertex);

      if(currentEntry.isEmpty())
      {
        continue;
      }

      if(graph.underlyingVertex(currentVertex) == source &&
         currentVertex.getTime() > 0)
      {
        timedTarget = currentVertex;
        found = true;
        break;
      }

      for(const TimedEdge& edge : graph.getOutgoing(currentVertex))
      {
        TimedVertex nextVertex = edge.getTarget();

        assert(currentFlow(edge) >= 0);

        if(!cmp::pos(currentFlow(edge)))
        {
          continue;
        }

        const double nextValue = std::min(currentEntry.value, currentFlow(edge));

        Entry& nextEntry = entries(nextVertex);

        if(nextEntry.isEmpty() || nextValue > nextEntry.value)
        {
          entries(nextVertex) = Entry(nextValue, edge);
        }
      }
    }

    // it can happen that the flow "disappears", if the (numerical) flow
    // value falls to zero
    if(!found)
    {
      break;
    }

    {
      TimedVertex currentVertex = timedTarget;
      TimedPath path;

      std::ostringstream buf;

      const double value = entries(currentVertex).value;

      buf << "Value: " << value << ", [";

      assert(cmp::pos(value));

      while(currentVertex != timedSource)
      {
        TimedEdge edge = entries(currentVertex).edge;

        path.prepend(edge);

        assert(currentFlow(edge) >= value);

        currentFlow(edge) = std::max(currentFlow(edge) - value, 0.);

        currentVertex = edge.getSource();
      }

      for(const TimedVertex& timedVertex : path.getVertices())
      {
        buf << graph.underlyingVertex(timedVertex) << ", ";
      }

      buf << "]";

      Log(debug) << buf.str();

      paths.push_back(path);

      remainingValue -= value;
    }
  }

  if(debuggingEnabled())
  {
    idx numRemaining = 0;

    for(const TimedEdge& edge : graph.getEdges())
    {
      if(cmp::pos(currentFlow(edge)))
      {
        ++numRemaining;
      }
    }

    if(numRemaining > 0)
    {
      Log(debug) << "Nonzero flow remaining on " << numRemaining << " edges";
    }
  }

  return paths;
}
