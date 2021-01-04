#include "exact_timed_solver.hh"

#include "log.hh"

#include "graph/subgraph.hh"

#include "simple_program.hh"

ExactTimedSolver::ExactTimedSolver(const Graph& graph,
                                   const std::vector<Vertex>& vertices)
  : graph(graph),
    vertices(vertices)
{
}

Tour ExactTimedSolver::findTour(TimedDistanceFunc& distances,
                                const Tour& initialTour)
{
  SubGraph subGraph = graph.reachableSubGraph(initialTour.getVertices());

  std::vector<Vertex> subVertices = ((const Graph& )subGraph).getVertices().collect();

  std::vector<Vertex> subTourVertices;

  for(idx i = 0; i < vertices.size(); ++i)
  {
    subTourVertices.push_back(subVertices[i]);
  }

  Tour subTour = Tour(subGraph, subTourVertices);

  assert(subTour.connects(subVertices));

  auto subFunc = ChainedTimedBiVertexFunc<int>(distances,
                                               [&](const Vertex& vertex) -> Vertex
                                               {
                                                 return subGraph.originalVertex(vertex);
                                               });

  SimpleProgram simpleProgram(subTour, subFunc);

  subTour = *(simpleProgram.solve().tour);

  std::vector<Vertex> tourVertices;

  for(const Vertex& subVertex: subTour.getVertices())
  {
    tourVertices.push_back(subGraph.originalVertex(subVertex));
  }

  return Tour(graph, tourVertices);
}
