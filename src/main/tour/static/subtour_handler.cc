#include "subtour_handler.hh"

#include <sstream>

#include "scip_utils.hh"

#include "flow/max_flow.hh"

#define CONS_HANDLER_NAME "atsp_subtour"

const double cutoff = 1e-6;

SubtourHandler::SubtourHandler(SCIP* scip,
                               const Graph& graph,
                               const EdgeMap<SCIP_VAR*>& variables)
  : scip::ObjConshdlr(scip,                                /**< SCIP data structure */
                      CONS_HANDLER_NAME,                   /**< name */
                      "ATSP subtour elimination handler",  /**< description */
                      1000000,                             /**< priority for separation */
                      -2000000,                            /**< priority for constraint enforcing */
                      -2000000,                            /**< priority for checking infeasibility */
                      1,                                   /**< frequency for separating cuts */
                      -1,                                  /**< frequency for propagating domains */
                      1,                                   /**< frequency for using all instead of only
                                                                the useful constraints in separation,
                                                                propagation and enforcement,
                                                               -1 for no eager evaluations, 0 for first only */
                      0,                                   /**< maximal number of presolving rounds the
                                                                constraint handler
                                                                participates in (-1: no limit) */
                      FALSE,                               /**< should separation method be delayed,
                                                                if other separators found cuts? */
                      FALSE,                               /**< should propagation method be delayed,
                                                                if other propagators found reductions? */
                      TRUE,                                /**< should the constraint handler be skipped,
                                                                if no constraints are available? */
                      SCIP_PROPTIMING_BEFORELP,            /**< positions in the node solving loop
                                                                where propagation method of constraint handlers
                                                                should be executed */
                      SCIP_PRESOLTIMING_FAST),             /**< timing mask of the constraint
                                                                handler's presolving method */
  graph(graph),
  variables(variables)
{
}

std::string SubtourHandler::getName() const
{
  return CONS_HANDLER_NAME;
}

SCIP_ROW* SubtourHandler::generateSubtourConstraint(SCIP* scip,
                                                    SCIP_CONSHDLR* conshdlr,
                                                    const VertexSet& vertices)
{
  std::ostringstream namebuf;

  SCIP_ROW* row;

  idx numEdges = 0, numVertices = 0;

  for(const Vertex& vertex : graph.getVertices())
  {
    if(vertices.contains(vertex))
    {
      ++numVertices;
    }
  }

  namebuf << "subtour_elimination_" << numVertices;

  SCIP_CALL_EXC(SCIPcreateEmptyRowConshdlr(scip,
                                           &row,
                                           conshdlr,
                                           namebuf.str().c_str(),
                                           1,
                                           SCIPinfinity(scip),
                                           FALSE,
                                           FALSE,
                                           TRUE));

  for(const Edge& edge : graph.getEdges())
  {
    if(edge.leaves(vertices))
    {
      SCIP_CALL_EXC(SCIPaddVarToRow(scip, row, variables(edge), 1.));
      ++numEdges;
    }
  }

  assert(numEdges > 0);

  Log(info) << "Created a cut with " << numEdges << " edges";

  return row;
}

SCIP_DECL_CONSSEPASOL(SubtourHandler::scip_sepasol)
{
  *result = SCIP_DIDNOTFIND;

  if(separate(scip, conshdlr))
  {
    *result = SCIP_SEPARATED;
  }

  return SCIP_OKAY;
}

SCIP_DECL_CONSSEPALP(SubtourHandler::scip_sepalp)
{
  *result = SCIP_DIDNOTFIND;

  if(separate(scip, conshdlr))
  {
    *result = SCIP_SEPARATED;
  }

  return SCIP_OKAY;
}

bool SubtourHandler::separate(SCIP* scip,
                              SCIP_CONSHDLR* conshdlr,
                              SCIP_SOL* sol)
{
  Log(info) << "Separating subtour constraints";

  EdgeSolutionValues solutionValues(scip, variables.getValues());

  MinCutResult cutResult = computeMinCut(graph,
                                         solutionValues);

  if(cutResult.value >= 1 - cutoff)
  {
    Log(info) << "Could not find cut";
    return false;
  }
  else
  {
    Log(info) << "Found a cut with value = " << cutResult.value;

    SCIP_ROW* row = generateSubtourConstraint(scip, conshdlr, cutResult.cut);

    SCIP_Bool infeasible;

    SCIP_CALL_EXC(SCIPaddRow(scip, row, FALSE, &infeasible));
    SCIP_CALL_EXC(SCIPreleaseRow(scip, &row));

    return true;
  }
}

bool SubtourHandler::findSubtour(SCIP* scip,
                                 SCIP_SOL* solution)
{
  Vertex first = graph.getVertices().collect().front();
  Vertex current = first;

  idx numVertices = graph.getVertices().size();
  idx currentCount = 0;

  while(true)
  {
    if(currentCount + 1 == numVertices)
    {
      break;
    }

    if(currentCount > 0 && current == first)
    {
      return true;
    }

    ++currentCount;

    bool found = false;

    for(const Edge& edge : graph.getOutgoing(current))
    {
      double value = SCIPgetSolVal(scip, solution, variables(edge));

      if(value >= 0.5)
      {
        Vertex next = edge.getTarget();

        current = next;
        found = true;

        break;
      }
    }

    assert(found);
  }

  return current == first;
}

SCIP_DECL_CONSLOCK(SubtourHandler::scip_lock)
{
  for(const Edge& edge : graph.getEdges())
  {
    if(variables(edge) != nullptr)
    {
      SCIP_CALL(SCIPaddVarLocks(scip, variables(edge), nlocksneg, nlockspos));
    }
  }

  return SCIP_OKAY;
}

SCIP_DECL_CONSCHECK(SubtourHandler::scip_check)
{
  Log(info) << "Checking solution";

  bool found = findSubtour(scip, sol);

  if(found)
  {
    Log(info) << "Found a subtour";
  }

  *result = found ? SCIP_INFEASIBLE : SCIP_FEASIBLE;

  return SCIP_OKAY;
}

SCIP_DECL_CONSENFOPS(SubtourHandler::scip_enfops)
{
  Log(info) << "scip_enfops";
  return SCIP_OKAY;
}

SCIP_DECL_CONSENFOLP(SubtourHandler::scip_enfolp)
{
  Log(info) << "Enforcing LP solution";

  for( int i = 0; i < nconss; ++i )
  {
    bool found = findSubtour(scip, NULL);

    if(found)
    {
      Log(info) << "Found a subtour";
    }

    *result = found ? SCIP_INFEASIBLE : SCIP_FEASIBLE;
  }

  return SCIP_OKAY;
}

SCIP_DECL_CONSTRANS(SubtourHandler::scip_trans)
{
  SCIP_CONSDATA* targetdata;

  targetdata = NULL;

  /* create target constraint */
  SCIP_CALL_EXC(SCIPcreateCons(scip,
                               targetcons,
                               SCIPconsGetName(sourcecons),
                               conshdlr,
                               targetdata,
                               SCIPconsIsInitial(sourcecons),
                               SCIPconsIsSeparated(sourcecons),
                               SCIPconsIsEnforced(sourcecons),
                               SCIPconsIsChecked(sourcecons),
                               SCIPconsIsPropagated(sourcecons),
                               SCIPconsIsLocal(sourcecons),
                               SCIPconsIsModifiable(sourcecons),
                               SCIPconsIsDynamic(sourcecons),
                               SCIPconsIsRemovable(sourcecons),
                               SCIPconsIsStickingAtNode(sourcecons)));

  return SCIP_OKAY;
}
