#include "cycle_separator.hh"

#include <sstream>

#include "scip_utils.hh"

#include "log.hh"

#include "timed/path_decomposition.hh"

#define NAME "Two-cycle separator"

CycleSeparator::CycleSeparator(SimpleProgram& program)
  : scip::ObjSepa(program.getSCIP(),
                  NAME,
                  "Separates cycle inequalities",
                  100000,
                  0,
                  0.0,
                  FALSE,
                  FALSE),
    program(program),
    graph(program.getGraph()),
    originalGraph(graph.underlyingGraph())
{
}

std::string CycleSeparator::getName() const
{
  return NAME;
}

SCIP_DECL_SEPAEXECLP(CycleSeparator::scip_execlp)
{
  Log(info) << "Starting cycle separation";
  idx numCuts = 0;

  auto paths = pathDecomposition(program.getGraph(),
                                 program.getSolutionValues(),
                                 program.getSource());

  for(const TimedPath& path : paths)
  {
    TimedPath cycle;
    TimedEdge incoming;

    if(hasCycle(path, incoming, cycle))
    {
      if(generateCut(scip, sepa, incoming, cycle))
      {
        ++numCuts;
      }
    }

  }

  Log(info) << "Found " << numCuts << " cuts";

  *result = (numCuts > 0) ? SCIP_SEPARATED : SCIP_DIDNOTFIND;

  return SCIP_OKAY;
}

bool CycleSeparator::generateCut(SCIP* scip,
                                 SCIP_SEPA* sepa,
                                 const TimedEdge& incoming,
                                 const TimedPath& cycle)
{
  assert(cycle.getEdges().size() >= 2);

  TimedVertex vertex = incoming.getTarget();

  assert(graph.underlyingVertex(cycle.getSource()) ==
         graph.underlyingVertex(cycle.getSource()));

  if(debuggingEnabled())
  {
    for(const TimedVertex& timedVertex : cycle.getVertices())
    {
      assert(graph.underlyingVertex(timedVertex) != program.getSource());
    }
  }

  assert(incoming.getTarget() == vertex);

  SCIP_ROW* cut;

  std::ostringstream namebuf;

  const EdgeMap<SCIP_VAR*>& variables = program.getVariables();

  namebuf << "cycle_"
          << graph.underlyingVertex(vertex).getIndex()
          << "#"
          << vertex.getTime()
          << "-"
          << (cycle.getEdges().size());

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),   // name
                                       0,                       // lhs
                                       SCIPinfinity(scip),      // rhs
                                       FALSE,                   // local
                                       FALSE,                   // modifiable
                                       TRUE));                  // removable

  SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(incoming), -1.));

  VertexSet visited(originalGraph);

  visited.insert(graph.underlyingVertex(incoming.getSource()));
  visited.insert(graph.underlyingVertex(incoming.getTarget()));

  for(const TimedEdge& cycleEdge : cycle.getEdges())
  {
    const TimedVertex& currentVertex = cycleEdge.getSource();
    const TimedVertex& nextVertex = cycleEdge.getTarget();

    assert(visited.contains(graph.underlyingVertex(currentVertex)));
    //assert(!visited.contains(graph.underlyingVertex(nextVertex)));

    visited.insert(graph.underlyingVertex(nextVertex));

    for(const TimedEdge& outgoing : graph.getOutgoing(currentVertex))
    {
      if(visited.contains(graph.underlyingVertex(outgoing.getTarget())))
      {
        continue;
      }

      SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(outgoing), 1.));
    }
  }

  idx infeasible = FALSE;

  bool efficatious = false;

  if(SCIPisCutEfficacious(scip, NULL, cut))
  {
    SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));

    efficatious = true;
  }

  SCIP_CALL_EXC(SCIPreleaseRow(scip, &cut));

  return efficatious;
}

bool CycleSeparator::hasCycle(const TimedPath& path,
                              TimedEdge& incoming,
                              TimedPath& cycle)
{
  VertexSet vertices(originalGraph);

  uint cycleSize = 0;
  cycle.clear();

  bool found = false;
  Vertex vertex;

  for(const TimedEdge& edge : path.getEdges())
  {
    Vertex originalTarget = graph.underlyingVertex(edge.getTarget());

    if(vertices.contains(originalTarget))
    {
      vertex = originalTarget;
      found = true;
      break;
    }
    else
    {
      vertices.insert(originalTarget);
    }
  }

  if(found)
  {
    std::ostringstream msgbuf;

    bool insert = false;
    for(const TimedEdge& edge : path.getEdges())
    {
      Vertex originalTarget = graph.underlyingVertex(edge.getTarget());

      if(insert)
      {
        if(debuggingEnabled())
        {
          msgbuf << graph.underlyingVertex(edge.getSource()).getIndex()
                 << "_"
                 << originalTarget.getIndex()
                 << "#"
                 << edge.getSource().getTime()
                 << ", ";
        }

        cycle.append(edge);
        ++cycleSize;

        if(originalTarget == vertex)
        {
          break;
        }
      }
      else
      {
        if(originalTarget == vertex)
        {
          if(debuggingEnabled())
          {
            msgbuf << " incoming: "
                   << graph.underlyingVertex(edge.getSource()).getIndex()
                   << "_"
                   << originalTarget.getIndex()
                   << "@"
                   << edge.getTarget().getTime()
                   << ", cycle: ";
          }

          incoming = edge;

          insert = true;
        }
      }
    }

    Log(debug) << "Found cycle of size "
               << cycle.getEdges().size()
               << " ("
               << msgbuf.str()
               << ")";

    if(debuggingEnabled())
    {
      assert(graph.underlyingVertex(cycle.getSource()) ==
             graph.underlyingVertex(cycle.getTarget()));

      VertexSet cycleVertices(originalGraph);

      for(const TimedEdge& timedEdge : cycle.getEdges())
      {
        assert(!cycleVertices.contains(graph.underlyingVertex(timedEdge.getTarget())));
        cycleVertices.insert(graph.underlyingVertex(timedEdge.getTarget()));
      }

      assert(!cycleVertices.contains(graph.underlyingVertex(incoming.getSource())));
      assert(!cycleVertices.contains(program.getSource()));

    }

    // only want proper cycles!
    if(cycleSize < originalGraph.getVertices().size())
    {
      return true;
    }

  }

  return false;
}
