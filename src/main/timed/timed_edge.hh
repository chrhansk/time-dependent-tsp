#ifndef TIMED_EDGE_HH
#define TIMED_EDGE_HH

#include "graph/edge.hh"

#include "timed_vertex.hh"

class TimedEdge
{
private:
  TimedVertex source, target;
  idx index;

public:
  TimedEdge(TimedVertex source,
            TimedVertex target,
            idx index);

  TimedEdge()
  {}

  TimedVertex getSource() const;

  TimedVertex getTarget() const;

  idx getIndex() const;

  idx travelTime() const;

  bool operator==(const TimedEdge& other) const;

  bool operator!=(const TimedEdge& other) const;

  operator Edge() const
  {
    return Edge(getSource(), getTarget(), getIndex());
  }

  bool enters(const VertexSet& vertices) const;

  bool leaves(const VertexSet& vertices) const;
};

namespace std
{
  template<>
  struct hash<TimedEdge>
  {
    typedef std::size_t result_type;

    result_type operator()(const TimedEdge& edge) const
    {
      result_type seed = 0;

      compute_hash_combination(seed, std::hash<TimedVertex>{}(edge.getSource()));
      compute_hash_combination(seed, std::hash<TimedVertex>{}(edge.getTarget()));

      return seed;
    }
  };
}


#endif /* TIMED_EDGE_HH */
