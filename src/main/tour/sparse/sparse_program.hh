#ifndef SPARSE_PROGRAM_HH
#define SPARSE_PROGRAM_HH

#include <map>
#include <vector>

#include <scip/scip.h>

#include <objscip/objscip.h>

#include "program.hh"
#include "scip_utils.hh"

#include "graph/graph.hh"
#include "graph/edge_set.hh"
#include "path/path.hh"

#include "timed/time_expanded_graph.hh"
#include "timed/timed_path.hh"
#include "timed/timed_vertex_func.hh"

#include "tour/tour.hh"
#include "tour/solution_result.hh"

#include "sparse_solution_values.hh"

class SparsePricer;
class SparsePricingManager;
class SparseSeparationManager;

class SparseProgram : public Program
{
protected:
  TimedDistanceFunc& distances;

  Tour initialTour;

  const Vertex source;
  TimeExpandedGraph graph;
  const Graph& originalGraph;
  idx timeHorizon;

  EdgeMap<SCIP_VAR*> combinedVariables;

  EdgeMap<SCIP_CONS*> linkingConstraints;

  VertexMap<SCIP_CONS*> coveringConstraints;


  SparseSeparationManager* separator;

  SparsePricingManager* pricer;

  Tour createTour(SCIP_SOL* sol) const;

  void createVariables();
  void createConstraints();

public:
  SparseProgram(const Tour& initialTour,
                TimedDistanceFunc& distances,
                num lowerBound = 0,
                bool withPricer = true,
                const Program::Settings& settings = Program::Settings());

  ~SparseProgram();

  SolutionResult solve(int timeLimit = -1);

  SolutionResult solveRelaxation(int timeLimit = -1);

  idx getTimeHorizon() const
  {
    return timeHorizon;
  }

  const EdgeMap<SCIP_VAR*>& getCombinedVariables() const
  {
    return combinedVariables;
  }

  bool hasFixedEdges() const;

  EdgeSet getForbiddenEdges() const;

  const VertexMap<SCIP_CONS*>& getCoveringConstraints() const
  {
    return coveringConstraints;
  }

  const EdgeMap<SCIP_CONS*>& getLinkingConstraints() const
  {
    return linkingConstraints;
  }

  const TimeExpandedGraph& getGraph() const
  {
    return graph;
  }

  Vertex getSource() const
  {
    return source;
  }

  const Tour& getInitialTour() const
  {
    return initialTour;
  }

  TimedDistanceFunc& getDistances()
  {
    return distances;
  }

  bool hasSeparator() const
  {
    return !!separator;
  }

  SparseSeparationManager& getSeparator() const
  {
    assert(separator);

    return *separator;
  }

  EdgeSolutionValues combinedSolutionValues() const
  {
    return EdgeSolutionValues(scip, combinedVariables.getValues());
  }

  EdgeMap<double> getDualValues(DualCostType costType) const;

  SparsePricingManager& getPricingManager() const
  {
    assert(pricer);

    return *pricer;
  }

  void setPricer(SparsePricer* pricer);

  void addHeuristic(scip::ObjHeur* heur);
  void addPropagator(scip::ObjProp* propagator);
  void addSeparator(SparseSeparationManager* separator);

  void addedEdge(const TimedEdge& timedEdge);

  SparseSolutionValues solutionValues() const;
};


#endif /* SPARSE_PROGRAM_HH */
