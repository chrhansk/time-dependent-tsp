#ifndef PATH_BASED_PROGRAM_HH
#define PATH_BASED_PROGRAM_HH

#include <map>
#include <vector>

#include <objscip/objscip.h>

#include "program.hh"

#include "graph/graph.hh"
#include "graph/edge_set.hh"
#include "path/path.hh"

#include "timed/timed_vertex_func.hh"

#include "tour/tour.hh"
#include "tour/solution_result.hh"

#include "timed/timed_path.hh"
#include "timed/time_expanded_graph.hh"

#include "tour/path/pricers/path_variable.hh"

class PathBasedPricer;
class CompoundBranching;

class PathBasedProgram : public Program
{
protected:
  TimedDistanceFunc& distances;
  Tour initialTour;
  Vertex source;

  const Graph& originalGraph;
  TimeExpandedGraph graph;

  const idx timeHorizon;

  PathBasedPricer* pricer;

  EdgeMap<SCIP_VAR*> combinedVariables;

  EdgeMap<SCIP_CONS*> linkingConstraints;

  VertexMap<SCIP_CONS*> coveringConstraints;

  void createVariables();
  void createConstraints();

public:
  PathBasedProgram(const Tour& initialTour,
                   TimedDistanceFunc& distances,
                   num lowerBound = 0,
                   bool withPricer = true,
                   const Program::Settings& settings = Program::Settings());

  const VertexMap<SCIP_CONS*>& getCoveringConstraints() const
  {
    return coveringConstraints;
  }

  const EdgeMap<SCIP_CONS*>& getLinkingConstraints() const
  {
    return linkingConstraints;
  }

  const EdgeMap<SCIP_VAR*>& getCombinedVariables() const
  {
    return combinedVariables;
  }

  idx getTimeHorizon() const
  {
    return timeHorizon;
  }

  EdgeMap<double> getFlow() const;

  const TimeExpandedGraph& getGraph() const
  {
    return graph;
  }

  const Graph& getOriginalGraph() const
  {
    return originalGraph;
  }

  Vertex getSource() const
  {
    return source;
  }

  TimedDistanceFunc& getDistances() const
  {
    return distances;
  }

  void setPricer(PathBasedPricer* pricer);

  const std::vector<PathVariable>& getPathVariables() const;

  SolutionResult solveRelaxation(int timeLimit = -1);

  SolutionResult solve(int timeLimit = -1);

  bool hasFixedEdges() const;

  EdgeSet getForbiddenEdges() const;

  EdgeSet getRequiredEdges() const;

  ~PathBasedProgram();
};


#endif /* PATH_BASED_PROGRAM_HH */
