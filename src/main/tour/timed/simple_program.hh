#ifndef SIMPLE_PROGRAM_HH
#define SIMPLE_PROGRAM_HH

#include <bitset>
#include <map>

#include <objscip/objscip.h>

#include "program.hh"
#include "scip_utils.hh"

#include "graph/graph.hh"

#include "timed/time_expanded_graph.hh"
#include "timed/timed_vertex_func.hh"

#include "tour/tour.hh"
#include "tour/solution_result.hh"

#include "timed/timed_path.hh"

class SimpleProgram : public Program
{
protected:
  TimeExpandedGraph graph;
  const Graph& originalGraph;
  TimedDistanceFunc& distances;

  Tour initialTour;

  EdgeMap<SCIP_VAR*> variables;

  Vertex source;

  void addInitialSolution(SCIP* scip,
                          const Tour& tour);


  void createVariables(SCIP* scip);

  void createFlowConstraints(SCIP* scip);

  void createCoveringConstraints(SCIP* scip);

  Tour createTour(SCIP_SOL* solution);

public:

  SimpleProgram(const Tour& initialTour,
                TimedDistanceFunc& distances,
                const Program::Settings& settings = Program::Settings());

  ~SimpleProgram();

  const TimeExpandedGraph& getGraph() const
  {
    return graph;
  }

  const EdgeMap<SCIP_VAR*>& getVariables() const
  {
    return variables;
  }

  const Tour& getInitialTour() const
  {
    return initialTour;
  }

  Vertex getSource() const
  {
    return source;
  }

  SolutionResult solveRelaxation(int timeLimit = -1);

  SolutionResult solve(int timeLimit = -1);

  TimedDistanceFunc& getDistances() const
  {
    return distances;
  }

  void addSeparator(scip::ObjSepa* separator);

  void addBranchingRule(scip::ObjBranchrule* rule);

  void addConstraintHandler(scip::ObjConshdlr* handler);

  EdgeSolutionValues getSolutionValues();

};

#endif /* SIMPLE_PROGRAM_HH */
