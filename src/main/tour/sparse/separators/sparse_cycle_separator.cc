#include "sparse_cycle_separator.hh"

#include <sstream>

#include "timed/path_decomposition.hh"

#include "tour/sparse/pricers/sparse_pricer.hh"

SparseCycleCut::SparseCycleCut(SparseSeparator& separator,
                               const TimedEdge& incoming,
                               const TimedPath& cycle,
                               SCIP_SEPA* sepa)
  : SparseCut(separator),
    graph(separator.getProgram().getGraph()),
    incoming(incoming),
    cycle(cycle),
    indices(graph.underlyingGraph(), -1)
{
  assert(incoming.getTarget() == cycle.getSource());

  assert(graph.underlyingVertex(cycle.getSource()) ==
         graph.underlyingVertex(cycle.getTarget()));

  createCut(sepa);
}

void SparseCycleCut::addIndices()
{
  idx index = 0;

  indices(graph.underlyingVertex(incoming.getSource())) = index++;
  indices(graph.underlyingVertex(incoming.getTarget())) = index++;

  for(const TimedEdge& cycleEdge : cycle.getEdges())
  {
    indices(graph.underlyingVertex(cycleEdge.getTarget())) = index++;
  }

  indices(graph.underlyingVertex(incoming.getTarget())) = 1;
}

void SparseCycleCut::createCut(SCIP_SEPA* sepa)
{
  addIndices();

  std::ostringstream namebuf;

  {
    const TimedVertex& timedVertex = incoming.getTarget();

    namebuf << "cycle_"
            << graph.underlyingVertex(timedVertex).getIndex()
            << "#"
            << timedVertex.getTime()
            << "-"
            << cycle.getEdges().size();
  }

  SCIP* scip = separator.getSCIP();

  assert(variables(incoming));

  SCIP_CALL_EXC(SCIPcreateEmptyRowSepa(scip,
                                       &cut,
                                       sepa,
                                       namebuf.str().c_str(),
                                       0,
                                       SCIPinfinity(scip),
                                       FALSE,  // local
                                       TRUE,   // modifiable
                                       TRUE)); // removable

  SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(incoming), -1.));

  /*
    Log(info) << "Incoming: "
    << graph.underlyingVertex(incoming.getSource())
    << "_"
    << graph.underlyingVertex(incoming.getTarget())
    << "#"
    << incoming.getSource().getTime();
  */

  for(const TimedEdge& cycleEdge : cycle.getEdges())
  {
    assert(variables(cycleEdge));

    num currentIndex = indices(graph.underlyingVertex(cycleEdge.getSource()));

    for(const TimedEdge& outgoing : graph.getOutgoing(cycleEdge.getSource()))
    {
      SCIP_VAR* variable = variables(outgoing);

      if(!variable)
      {
        continue;
      }

      if(outgoing == cycleEdge)
      {
        continue;
      }

      num targetIndex = indices(graph.underlyingVertex(outgoing.getTarget()));

      if(targetIndex == -1 || targetIndex > currentIndex)
      {
        SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variable, 1.));

        /*
          Log(info) << "Adjacent: "
          << graph.underlyingVertex(outgoing.getSource())
          << "_"
          << graph.underlyingVertex(outgoing.getTarget())
          << "#"
          << outgoing.getSource().getTime();
        */
      }
    }
  }

  idx infeasible = FALSE;

  SCIP_CALL_EXC(SCIPaddRow(scip, cut, FALSE, &infeasible));
}

void SparseCycleCut::addedEdge(const TimedEdge& timedEdge)
{
  SCIP* scip = separator.getSCIP();

  for(const TimedEdge& cycleEdge : cycle.getEdges())
  {
    assert(timedEdge != cycleEdge);

    if(timedEdge.getSource() == cycleEdge.getSource())
    {
      num currentIndex = indices(graph.underlyingVertex(cycleEdge.getSource()));
      num targetIndex = indices(graph.underlyingVertex(timedEdge.getTarget()));

      if(targetIndex == -1 || targetIndex > currentIndex)
      {
        SCIP_CALL_EXC(SCIPaddVarToRow(scip, cut, variables(timedEdge), 1.));
      }

      break;
    }
  }
}

void SparseCycleCut::addDualCosts(EdgeMap<double>& dualCosts, DualCostType costType) const
{
  const double costCoefficient = getDual(costType);

  dualCosts(incoming) += (-costCoefficient);

  for(const TimedEdge& cycleEdge : cycle.getEdges())
  {
    num currentIndex = indices(graph.underlyingVertex(cycleEdge.getSource()));

    for(const TimedEdge& outgoing : graph.getOutgoing(cycleEdge.getSource()))
    {
      if(outgoing == cycleEdge)
      {
        continue;
      }

      num targetIndex = indices(graph.underlyingVertex(outgoing.getTarget()));

      if(targetIndex == -1 || targetIndex > currentIndex)
      {
        dualCosts(outgoing) += costCoefficient;
      }
    }
  }
}

std::vector<std::unique_ptr<SparseCut>>
SparseCycleSeparator::separate(const EdgeFunc<double>& values,
                               SCIP_SEPA* sepa,
                               int maxNumCuts)
{
  auto paths = pathDecomposition(program.getGraph(),
                                 program.solutionValues(),
                                 program.getSource());

  std::vector<std::unique_ptr<SparseCut>> cuts{};

  int foundCuts = 0;

  for(const TimedPath& path : paths)
  {
    TimedPath cycle;
    TimedEdge incoming;

    if(hasCycle(path, incoming, cycle))
    {
      cuts.push_back(std::make_unique<SparseCycleCut>(*this,
                                                      incoming,
                                                      cycle,
                                                      sepa));

      if(maxNumCuts != -1 && ++foundCuts >= maxNumCuts)
      {
        break;
      }
    }
  }

  return cuts;
}


bool SparseCycleSeparator::hasCycle(const TimedPath& path,
                                    TimedEdge& incoming,
                                    TimedPath& cycle)
{
  const TimeExpandedGraph& graph = program.getGraph();
  const Graph& originalGraph = graph.underlyingGraph();

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
