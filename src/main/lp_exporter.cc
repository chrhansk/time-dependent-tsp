#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>

#include <boost/program_options.hpp>
namespace po = boost::program_options;


#include "util.hh"
#include "log.hh"

#include "tour/static/tour_solver.hh"

#include "tour/timed/expand_tour.hh"
#include "tour/timed/heuristics/shifting_solver.hh"

#include "tour/timed/separators/separators.hh"

#include "tour/timed/covering_relaxation.hh"

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/heuristics/greedy_constructions.hh"

#include "tour/sparse/pricers/sparse_stabilizing_pricer.hh"

#include "tour/path/path_based_program.hh"

#include "timed/cached_distances.hh"
#include "timed/cached_tree_distances.hh"
#include "timed/augmented_edge_func.hh"

#include "tour/sparse/sparse_objective_propagator.hh"

#include "instance.hh"

int main(int argc, char **argv)
{
  logInit();

  po::options_description desc("Allowed options");

  desc.add_options()
    ("help", "produce help message")
    ("seed", po::value<idx>(), "seed")
    ("size", po::value<idx>(), "number of vertices")
    ("output", po::value<std::string>(), "output");

  po::positional_options_description pos;
  pos.add("output", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
            .options(desc)
            .positional(pos)
            .run(),
            vm);

  po::notify(vm);

  if(vm.count("help"))
  {
    std::cout << desc << std::endl;
    return 1;
  }

  idx numVertices = 50;
  idx seed = 0;

  if(vm.count("size"))
  {
    numVertices = vm["size"].as<idx>();
  }

  if(vm.count("seed"))
  {
    seed = vm["seed"].as<idx>();
  }

  std::string output = vm["output"].as<std::string>();

  InstanceInfo info(seed, numVertices);
  Instance instance(info);

  TourSolver simpleSolver(instance.graph, instance.staticCosts);

  Tour initialTour = simpleSolver.findTour();

  num staticCost = initialTour.cost(instance.staticCosts);

  Log(info) << "Costs of initial tour according to static cost function: "
            << staticCost;

  TimedDistanceEvaluator evaluator(instance.timedDistances);

  num initialTourCost = evaluator(initialTour);

  Log(info) << "Costs of initial tour according to timed cost function: "
            << initialTourCost;

  SimpleProgram program(initialTour, instance.timedDistances);

  program.writeOriginal(output);

  return 0;
}
