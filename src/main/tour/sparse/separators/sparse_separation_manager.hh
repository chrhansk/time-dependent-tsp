#ifndef SPARSE_SEPARATION_MANAGER_HH
#define SPARSE_SEPARATION_MANAGER_HH

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#include <objscip/objscip.h>

#include "scip_utils.hh"

#include "sparse_cut.hh"
#include "sparse_separator.hh"

class SparseSeparationManager : public scip::ObjSepa
{
private:
  SparseProgram& program;
  num maxRounds;
  num maxCutsPerRound;
  num currentRound;

  std::vector<std::unique_ptr<SparseCut>> cuts;
  std::vector<std::unique_ptr<SparseSeparator>> separators;

public:
  SparseSeparationManager(SparseProgram& program,
                          num maxRounds = 10,
                          num maxCutsPerRound = 50);

  void addSeparator(std::unique_ptr<SparseSeparator>&& separator);

  SCIP_DECL_SEPAEXECLP(scip_execlp) override;

  SCIP_DECL_SEPAEXITSOL(scip_exitsol) override;

  void addedEdge(const TimedEdge& timedEdge);

  void addDualCosts(EdgeMap<double>& dualCosts,
                    DualCostType costType) const;

  const SparseProgram& getProgram() const
  {
    return program;
  }

  bool hasCuts() const
  {
    return !cuts.empty();
  }
};


#endif /* SPARSE_SEPARATION_MANAGER_HH */
