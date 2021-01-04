#ifndef SPARSE_PRICING_MANAGER_HH
#define SPARSE_PRICING_MANAGER_HH

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <objscip/objscip.h>

#include "scip_utils.hh"

#include "graph/graph.hh"
#include "tour/tour.hh"

#include "timed/timed_path.hh"

#include "sparse_pricer.hh"

class SparseProgram;
class SparsePricingResult;

class SparsePricingManager : public scip::ObjPricer
{
protected:
  SparseProgram& program;
  std::unique_ptr<SparsePricer> sparsePricer;

  SCIP* scip;

  const TimeExpandedGraph& graph;
  TimedDistanceFunc& distances;
  idx timeHorizon;

  Vertex source;

  VertexMap<SCIP_CONS*> coveringConstraints;
  EdgeMap<SCIP_CONS*> linkingConstraints;

  VertexMap<SCIP_CONS*> flowConstraints;
  EdgeMap<SCIP_VAR*> variables;

  bool initiated;

  void addVertex(const TimedVertex& timedVertex);

  bool addSolution(const TimedPath& path,
                   SCIP_HEUR* heur = nullptr);

  SCIP_VAR* addEdge(const TimedEdge& timedEdge);

  void addResult(const SparsePricingResult& result,
                 DualCostType dualCostType,
                 double* lowerbound);

public:
  SparsePricingManager(SparseProgram& program,
                       std::unique_ptr<SparsePricer>&& sparsePricer);

  SparsePricingManager(SparseProgram& program);

  virtual ~SparsePricingManager() {}

  bool addTour(const Tour& tour, SCIP_HEUR* heur = nullptr);

  void addPath(const TimedPath& path);

  std::string getName() const;

  bool contains(const TimedVertex& timedVertex) const;

  bool contains(const TimedEdge& timedEdge) const;

  const EdgeMap<SCIP_VAR*>& getVariables() const
  {
    return variables;
  }

  const VertexMap<SCIP_CONS*>& getFlowConstraints() const
  {
    return flowConstraints;
  }

  const EdgeMap<SCIP_CONS*>& getLinkingConstraints() const
  {
    return linkingConstraints;
  }

  const VertexMap<SCIP_CONS*>& getCoveringConstraints() const
  {
    return coveringConstraints;
  }

  void setPricer(std::unique_ptr<SparsePricer>&& pricer)
  {
    sparsePricer = std::move(pricer);
  }

  SCIP* getSCIP() const
  {
    return scip;
  }

  const TimeExpandedGraph& getGraph() const
  {
    return graph;
  }

  SparseProgram& getProgram()
  {
    return program;
  }

  virtual SCIP_DECL_PRICERINIT(scip_init) override;

  virtual SCIP_DECL_PRICEREXIT(scip_exit) override;

  virtual SCIP_DECL_PRICERFARKAS(scip_farkas) override;

  virtual SCIP_DECL_PRICERREDCOST(scip_redcost) override;
};


#endif /* SPARSE_PRICING_MANAGER_HH */
