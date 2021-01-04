#ifndef PROGRAM_BENCHMARK_HH
#define PROGRAM_BENCHMARK_HH

#include "tour/tour.hh"
#include "tour/solution_result.hh"

#include "instance.hh"

#include "timer.hh"

class ProgramBenchmark
{
protected:
  virtual SolutionResult execute(Instance& instance,
                                 const Tour& initialTour,
                                 int timeLimit = -1) = 0;

public:
  void executeAll(const std::vector<InstanceInfo>& instanceInfos = InstanceInfo::smallInstances());

  void run(int argc, char *argv[]);

  static std::string setFilePath(const std::string& name);
};


#endif /* PROGRAM_BENCHMARK_HH */
