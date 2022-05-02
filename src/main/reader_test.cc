#include <algorithm>

#include "timed/cached_tree_distances.hh"
#include "router/router.hh"
#include "tour/sparse/sparse_program.hh"
#include "tour/static/tour_solver.hh"
#include "timed/timed_reader.hh"

int main(int argc, char *argv[])
{
  logInit();

  std::ifstream input (argv[1], std::ifstream::in);

  TimedReadResult readResult = readTimedInstance(input);

  /*
  const Graph& graph = readResult.graph;

  EdgeMap<num> staticCosts(graph, 0);

  for(const Edge& edge : graph.getEdges())
  {
    const std::vector<num>& edgeValues = readResult.travelTimes.edgeValues(edge);

    staticCosts(edge) = *std::min_element(std::begin(edgeValues),
                                          std::end(edgeValues));
  }

  Dijkstra<> dijkstra(graph);
  ShortestPathDistances<> distances(dijkstra, staticCosts.getValues());

  TourSolver simpleSolver(graph, distances);

  Tour initialTour = simpleSolver.findTour();

  num staticCost = initialTour.cost(staticCosts.getValues());

  Log(info) << "Costs of initial tour according to static cost function: "
            << staticCost;

  CachedTreeDistances cachedDistances(graph, graph.getVertices().collect(), readResult.travelTimes);

  SparseProgram program(initialTour,
                        cachedDistances,
                        staticCost);
  */

  return 0;
}
