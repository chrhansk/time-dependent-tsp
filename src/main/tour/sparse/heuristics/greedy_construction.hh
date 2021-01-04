#ifndef GREEDY_CONSTRUCTION_HH
#define GREEDY_CONSTRUCTION_HH

#include <random>

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <objscip/objscip.h>

#include "tour/sparse/sparse_program.hh"
#include "tour/sparse/pricers/sparse_pricing_manager.hh"

#include "graph/edge_set.hh"

class GreedyConstruction : public scip::ObjHeur
{
protected:
  SparseProgram& program;
  const TimeExpandedGraph& graph;
  const Graph& originalGraph;
  const Vertex source;

  int numRuns;
  int currentSeed;

  bool addAsTour(const TimedPath& timedPath,
                 SCIP_HEUR* heur);

  TimedEdge bestEdge(const TimedVertex& source,
                     const std::vector<TimedEdge>& validEdges) const;

  TimedEdge selectEdge(const TimedVertex& source,
                       const std::vector<TimedEdge>& validEdges,
                       std::default_random_engine& engine) const;

  std::vector<TimedEdge> validEdges(const TimedVertex& current,
                                    const VertexSet& visited,
                                    const EdgeSet& forbidden) const;

  bool constructTour(const EdgeSet& forbidden,
                     TimedPath& path,
                     std::default_random_engine& engine);

  bool constructBestTour(const EdgeSet& forbidden,
                         TimedPath& path);

  virtual double score(const TimedEdge& edge) const = 0;

  virtual bool startRound()
  {
    return true;
  }

public:
  GreedyConstruction(SparseProgram& program,
                     const std::string& name,
                     const std::string& description,
                     char symbol,
                     int numRuns);

  SCIP_DECL_HEUREXEC(scip_exec) override;
};


#endif /* GREEDY_CONSTRUCTION_HH */
