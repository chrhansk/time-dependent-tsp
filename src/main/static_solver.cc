#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>

#include <boost/program_options.hpp>
namespace po = boost::program_options;


#include "util.hh"
#include "log.hh"

#include "tour/tour.hh"
#include "tour/static/tour_solver.hh"
#include "tour/static/separators/static_dk_separator.hh"
#include "tour/static/separators/static_odd_cat_separator.hh"

std::vector<Vertex> generateRandomVertices(const Graph& graph,
                                           idx numVertices,
                                           long seed = 12)
{
  std::vector<Vertex> vertices = graph.getVertices().collect();

  std::mt19937 engine(seed);
  std::shuffle(std::begin(vertices), std::end(vertices), engine);

  if(numVertices > vertices.size())
  {
    throw std::invalid_argument("Insufficient number of vertices");
  }

  vertices.resize(numVertices);

  return vertices;
}

int main(int argc, char **argv)
{
  logInit();

  po::options_description desc("Allowed options");

  desc.add_options()
    ("help", "produce help message")
    ("size", po::value<unsigned int>(), "number of vertices");

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
            .options(desc).run(),
            vm);

  po::notify(vm);

  if(vm.count("help"))
  {
    std::cout << desc << std::endl;
    return 1;
  }

  unsigned int numVertices = 50;

  if(vm.count("size"))
  {
    numVertices = vm["size"].as<unsigned int>();
  }

  Graph graph = Graph::complete(numVertices);

  std::mt19937 engine(0);

  auto coordinates = std::uniform_int_distribution<>(0, 100);

  VertexMap<Point> points(graph, Point(0, 0));

  for(const Vertex& vertex : graph.getVertices())
  {
    points(vertex) = Point(coordinates(engine), coordinates(engine));
  }

  EdgeMap<num> costs(graph, 0);

  for(const Edge& edge : graph.getEdges())
  {
    costs(edge) = (num) points(edge.getSource()).distance(points(edge.getTarget()));
  }

  Dijkstra<> dijkstra(graph);
  ShortestPathDistances<> distances(dijkstra, costs.getValues());

  TourSolver solver(graph, distances);

  Log(info) << "Finding a tour with " << numVertices << " vertices";

  //solver.addSeparator(new StaticDKSeparator(solver));
  solver.addSeparator(new StaticOddCATSeparator(solver));

  Tour tour = solver.findTour();

  Log(info) << "Tour has a cost of " << tour.cost(distances) << "";

  return 0;
}
