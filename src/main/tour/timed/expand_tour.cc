#include "expand_tour.hh"

#include <queue>

#include "log.hh"

struct VertexComparator
{
  bool operator()(const TimedVertex& first,
                  const TimedVertex& second) const
  {
    return first.getTime() > second.getTime();
  }
};

TimeExpandedGraph createTimeExpandedGraph(const Tour& tour,
                                          TimedDistanceFunc& distances,
                                          idx lowerBound)
{
  // TODO: Add predecessors

  const Graph& originalGraph = tour.getGraph();

  TimeExpandedGraph graph(originalGraph);
  const idx timeHorizon = tour.cost(distances);

  Log(info) << "Expanding a tour within a time horizon of " << timeHorizon
            << " on " << originalGraph.getVertices().size()
            << " vertices";

  std::priority_queue<TimedVertex,
                      std::vector<TimedVertex>,
                      VertexComparator> unprocessed;

  const Vertex initialVertex = tour.getSource();

  unprocessed.push(graph.addVertex(initialVertex, 0));

  while(!unprocessed.empty())
  {
    const TimedVertex currentVertex = unprocessed.top();
    const Vertex sourceVertex = graph.underlyingVertex(currentVertex);

    unprocessed.pop();

    assert(graph.getOutgoing(currentVertex).empty());

    if(sourceVertex == initialVertex && currentVertex.getTime() > 0)
    {
      continue;
    }

    const idx departureTime = currentVertex.getTime();

    for(const Edge& outgoing : originalGraph.getOutgoing(sourceVertex))
    {
      const Vertex& targetVertex = outgoing.getTarget();

      if(targetVertex == graph.underlyingVertex(currentVertex))
      {
        continue;
      }

      const idx arrivalTime = departureTime + distances(sourceVertex,
                                                        targetVertex,
                                                        departureTime);

      assert(arrivalTime > departureTime);

      if(targetVertex == initialVertex && arrivalTime < lowerBound)
      {
        continue;
      }

      if(arrivalTime > timeHorizon)
      {
        continue;
      }

      const idx sourceTime = arrivalTime + distances(targetVertex,
                                                     initialVertex,
                                                     arrivalTime);

      if(sourceTime > timeHorizon)
      {
        continue;
      }

      if(graph.hasVertex(targetVertex, arrivalTime))
      {
        graph.addEdge(currentVertex, graph.getVertex(targetVertex, arrivalTime), outgoing);
      }
      else
      {
        const TimedVertex nextVertex = graph.addVertex(targetVertex, arrivalTime);
        graph.addEdge(currentVertex, nextVertex, outgoing);
        unprocessed.push(nextVertex);
      }
    }
  }

  Log(info) << "Expanded graph has " << graph.getVertices().size()
            << " vertices and " << graph.getEdges().size()
            << " edges";

  return graph;
}
