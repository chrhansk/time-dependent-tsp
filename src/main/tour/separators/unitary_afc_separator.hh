#ifndef UNITARY_AFC_SEPARATOR_HH
#define UNITARY_AFC_SEPARATOR_HH

#include "graph/vertex_set.hh"
#include "timed/time_expanded_graph.hh"

class UnitaryAFCSet
{
private:
  TimedEdge incoming;
  VertexSet cut;
public:
  UnitaryAFCSet(const TimedEdge& incoming,
                const VertexSet& cut)
    : incoming(incoming),
      cut(cut)
  {}

  const TimedEdge& getIncoming() const
  {
    return incoming;
  }

  const VertexSet& getCut() const
  {
    return cut;
  }
};


class UnitaryAFCSeparator
{
private:
  const TimeExpandedGraph& graph;
  const Graph& originalGraph;
  Vertex originalSource;

  bool findCut(const TimedEdge& incoming,
               const EdgeFunc<double>& weights,
               VertexSet& cut) const;

  VertexSet findVertices(const TimedEdge& incoming) const;

public:
  UnitaryAFCSeparator(const TimeExpandedGraph& graph,
                      const Vertex& originalSource);

  std::vector<UnitaryAFCSet> separate(const EdgeFunc<double>& weights,
                                      int maxNumSets = -1);

};


#endif /* UNITARY_AFC_SEPARATOR_HH */
