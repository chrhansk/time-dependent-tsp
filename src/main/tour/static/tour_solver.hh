#ifndef TOUR_SOLVER_HH
#define TOUR_SOLVER_HH

#include <scip/scip.h>

#include "graph/graph.hh"
#include "graph/edge_map.hh"

#include "router/router.hh"

#include "tour/tour.hh"

#include "program.hh"

class TourSolver
{
private:
  const Graph& graph;
  DistanceFunc& distances;
  Program program;
  SCIP* scip;
  EdgeMap<SCIP_VAR*> variables;

  void createVariables(SCIP* scip);

  void createFlowConstraints(SCIP* scip);

  Tour createTour(SCIP_SOL* solution);

  void addSubtourConstraint(const std::string& handlerName);

public:
  TourSolver(const Graph& graph,
             DistanceFunc& distances,
             const Program::Settings& settings = Program::Settings());

  ~TourSolver();

  Tour findTour();

  SCIP* getSCIP()
  {
    return scip;
  }

  const EdgeMap<SCIP_VAR*>& getVariables() const
  {
    return variables;
  }

  EdgeMap<SCIP_VAR*>& getVariables()
  {
    return variables;
  }

  const Graph& getGraph() const
  {
    return graph;
  }

  void addSeparator(scip::ObjSepa* separator);
};

#endif /* TOUR_SOLVER_HH */
