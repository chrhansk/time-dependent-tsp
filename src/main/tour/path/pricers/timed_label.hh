#ifndef TIMED_LABEL_HH
#define TIMED_LABEL_HH

#include <optional>

#include "graph/graph.hh"
#include "timed/time_expanded_graph.hh"

class TimedLabel
{
private:
  double cost;
  idx time;
  TimedEdge edge;
  std::shared_ptr<TimedLabel> predecessor;

public:
  TimedLabel(const TimedEdge& edge,
             double cost,
             idx time,
             std::shared_ptr<TimedLabel> predecessor)
    : cost(cost),
      time(time),
      edge(edge),
      predecessor(predecessor)
  {}

  TimedLabel(TimedVertex vertex,
             double cost)
    : cost(cost),
      time(0),
      edge(vertex, vertex, -1)
  {}

  idx getTime() const
  {
    return time;
  }

  double getCost() const
  {
    return cost;
  }

  TimedVertex getVertex() const
  {
    return edge.getTarget();
  }

  const TimedEdge& getEdge() const
  {
    return edge;
  }

  std::shared_ptr<TimedLabel> getPredecessor() const
  {
    return predecessor;
  }

};

namespace std
{
  /**
   * A hash function for timed labels
   */
  template<> struct hash<TimedLabel>
  {
    typedef TimedLabel argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& label) const
    {
      result_type seed = 0;
      compuate_hash_combination(seed, label.getVertex());
      compuate_hash_combination(seed, label.getTime());
      return seed;
    }
  };
}


#endif /* TIMED_LABEL_HH */
