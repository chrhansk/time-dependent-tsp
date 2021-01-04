#include "simple_unitary_afc_separator.hh"

#include <sstream>

#include "flow/max_flow.hh"

#include "scip_utils.hh"

#include "log.hh"

#define NAME "Unitary AFC separator"

SimpleUnitaryAFCSeparator::SimpleUnitaryAFCSeparator(SimpleProgram& program)
  : scip::ObjSepa(program.getSCIP(),
                  NAME,
                  "Separates unitary AFC inequalities",
                  100000,
                  0,
                  0.0,
                  FALSE,
                  FALSE),
  program(program),
  separator(program.getGraph(), program.getSource())
{
}

void SimpleUnitaryAFCSeparator::generateCut(SCIP* scip,
                                            SCIP_SEPA* sepa,
                                            const UnitaryAFCSet& set)
{
  SCIP_ROW* row;

  std::ostringstream namebuf;

  TimedEdge incoming = set.getIncoming();
  const VertexSet& cut = set.getCut();

  const TimeExpandedGraph& graph = program.getGraph();

  const Vertex& originalSource = graph.underlyingVertex(incoming.getSource());
  const Vertex& originalTarget = graph.underlyingVertex(incoming.getTarget());

  const EdgeMap<SCIP_VAR*>& variables = program.getVariables();

  namebuf << "unitary_afc_"
          << originalSource.getIndex()
          << "#"
          << incoming.getSource().getTime()
          << "_"
          << originalTarget.getIndex()
          << "#"
          << incoming.getTarget().getTime();

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &row,
                                       sepa,
                                       namebuf.str().c_str(),
                                       0,
                                       SCIPinfinity(scip),
                                       FALSE,
                                       FALSE,
                                       TRUE));

  SCIP_CALL_EXC(SCIPaddVarToRow(scip, row, variables(incoming), -1.));

  for(const TimedEdge& edge : graph.getEdges())
  {
    if(!edge.leaves(cut))
    {
      continue;
    }

    const Vertex& source = graph.underlyingVertex(edge.getSource());
    const Vertex& target = graph.underlyingVertex(edge.getTarget());

    if(source == originalTarget)
    {
      assert(edge.getSource() == incoming.getTarget());
    }

    assert(source != originalSource);

    if(target == originalSource || target == originalTarget)
    {
      continue;
    }

    SCIP_CALL_EXC(SCIPaddVarToRow(scip, row, variables(edge), 1.));
  }

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, row, FALSE, &infeasible));

  SCIP_CALL_EXC(SCIPreleaseRow(scip, &row));
}

std::string SimpleUnitaryAFCSeparator::getName() const
{
  return NAME;
}

SCIP_DECL_SEPAEXECLP(SimpleUnitaryAFCSeparator::scip_execlp)
{
  EdgeSolutionValues weights = program.getSolutionValues();

  *result = SCIP_DIDNOTRUN;

  std::vector<UnitaryAFCSet> sets = separator.separate(weights);

  for( const UnitaryAFCSet& set : sets)
  {
    generateCut(scip, sepa, set);
  }

  *result = (sets.size() > 0) ? SCIP_SEPARATED : SCIP_DIDNOTFIND;

  return SCIP_OKAY;
}
