#include "program_benchmark.hh"

#include <sstream>
#include <stdexcept>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "defs.hh"
#include "defs.hh"
#include "lp_observer.hh"

#include "tour/timed/expand_tour.hh"
#include "tour/static/tour_solver.hh"

void ProgramBenchmark::executeAll(const std::vector<InstanceInfo>& instanceInfos)
{
  std::cout << "Size;Seed;initialTourCost;numExpandedVertices;numExpandedEdges;Time;primalBound;dualBound;gap;"
            << "estimatedTreeSize;maxDepth;numIterations;numNodes;numVariables;"
            << "numConstraints;LPRootObjVal;rootLPSolved;rootNodeSolvingTime;"
            << "firstLPTime;firstDualBoundRoot;firstLowerBoundRoot;"
            << "minRows;maxRows;avgRows;"
            << "minCols;maxCols;avgCols;numLPs;numCuts"
            << std::endl;

  for(const InstanceInfo& instanceInfo : instanceInfos)
  {
    Instance instance(instanceInfo);

    TourSolver simpleSolver(instance.graph, instance.staticCosts);

    Tour initialTour = simpleSolver.findTour();

    idx timeLimit = 3600;

    Timer timer;

    auto result = execute(instance, initialTour, timeLimit);

    const double elapsed = timer.elapsed();

    TimedDistanceEvaluator evaluator(instance.timedDistances);

    const double initialTourCost = evaluator(initialTour);

    const SolutionStats& stats = result.stats;


    TimeExpandedGraph expandedGraph = createTimeExpandedGraph(initialTour,
                                                              instance.timedDistances);

    const Graph& graph = expandedGraph;

    std::cout << instanceInfo.numVertices
              << ";"
              << instanceInfo.seed
              << ";"
              << initialTourCost
              << ";"
              << graph.getVertices().size()
              << ";"
              << graph.getEdges().size()
              << ";"
              << std::min(elapsed, (double) timeLimit)
              << ";"
              << stats.primalBound
              << ";"
              << stats.dualBound
              << ";"
              << stats.gap
              << ";"
              << stats.estimatedTreeSize
              << ";"
              << stats.maxDepth
              << ";"
              << stats.numIterations
              << ";"
              << stats.numNodes
              << ";"
              << stats.numVariables
              << ";"
              << stats.numConstraints
              << ";"
              << stats.lpStats.LProotObjVal
              << ";"
              << stats.lpStats.rootLPSolved
              << ";"
              << stats.lpStats.rootSolvingTime
              << ";"
              << stats.firstLPTime
              << ";"
              << stats.firstDualBoundRoot
              << ";"
              << stats.firstLowerBoundRoot
              << ";"
              << stats.lpStats.minRows
              << ";"
              << stats.lpStats.maxRows
              << ";"
              << stats.lpStats.avgRows
              << ";"
              << stats.lpStats.minCols
              << ";"
              << stats.lpStats.maxCols
              << ";"
              << stats.lpStats.avgCols
              << ";"
              << stats.lpStats.numLPs
              << ";"
              << stats.numCuts
              << ";"
              << std::endl;

  }
}

std::string ProgramBenchmark::setFilePath(const std::string& name)
{
  std::ostringstream namebuf;

#if !defined(BENCH_SETTINGS_DIRECTORY)
  #error "Settings directory is not defined"
#endif

  namebuf << BENCH_SETTINGS_DIRECTORY;
  namebuf << "/";
  namebuf << name;
  namebuf << ".set";

  return namebuf.str();
}


void ProgramBenchmark::run(int argc, char *argv[])
{
  logInit();

  po::options_description desc("Allowed options");

  desc.add_options()
    ("help", "produce help message")
    ("size", po::value<std::string>(), "size ");

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
            .options(desc).run(),
            vm);

  po::notify(vm);

  if(vm.count("help"))
  {
    std::cout << desc << std::endl;
    return;
  }

  std::vector<InstanceInfo> instanceInfos;

  std::string size = "small";

  if(vm.count("size"))
  {
    size = vm["size"].as<std::string>();
  }

  if(size == "tiny")
  {
    instanceInfos = InstanceInfo::tinyInstances();
  }
  else if(size == "small")
  {
    instanceInfos = InstanceInfo::smallInstances();
  }
  else if(size == "medium")
  {
    instanceInfos = InstanceInfo::mediumInstances();
  }
  else if(size == "large")
  {
    instanceInfos = InstanceInfo::largeInstances();
  }
  else
  {
    throw std::logic_error("Invalid size");
  }

  executeAll(instanceInfos);
}
