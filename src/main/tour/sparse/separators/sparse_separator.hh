#ifndef SPARSE_SEPARATOR_HH
#define SPARSE_SEPARATOR_HH

#include "tour/sparse/sparse_program.hh"

#include "sparse_cut.hh"

class SparseSeparator
{
protected:
  SparseProgram& program;
  SCIP* scip;

public:
  SparseSeparator(SparseProgram& program)
    : program(program),
      scip(program.getSCIP())
  {}

  virtual ~SparseSeparator() {}

  SCIP* getSCIP() const
  {
    return scip;
  }

  virtual std::vector<std::unique_ptr<SparseCut>> separate(const EdgeFunc<double>& values,
                                                           SCIP_SEPA* sepa,
                                                           int maxNumCuts = -1) = 0;

  const SparseProgram& getProgram() const
  {
    return program;
  }
};


#endif /* SPARSE_SEPARATOR_HH */
