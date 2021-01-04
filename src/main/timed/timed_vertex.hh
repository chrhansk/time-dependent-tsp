#ifndef TIMED_VERTEX_HH
#define TIMED_VERTEX_HH

#include "util.hh"

#include "graph/vertex.hh"

class TimedVertex
{
private:
  idx index, time;

public:
  /**
   * Constructs a new vertex.
   *
   * @param index the index of the vertex
   */
  explicit TimedVertex(idx index, idx time)
    : index(index), time(time) {}

  TimedVertex()
  {}

  bool operator==(const TimedVertex& other) const;
  bool operator!=(const TimedVertex& other) const;

  /**
   * A comparison operator, based on indices.
   */
  bool operator<(const TimedVertex& other) const;

  /**
   * Returns the index of this vertex.
   */
  idx getIndex() const;

  operator Vertex() const
  {
    return Vertex(getIndex());
  }

  idx getTime() const;
};

namespace std {

  template <>
  struct hash<TimedVertex>
  {
    typedef std::size_t result_type;

    result_type operator()(const TimedVertex& vertex) const
    {
      result_type seed = 0;

      compute_hash_combination(seed, std::hash<idx>{}(vertex.getIndex()));
      compute_hash_combination(seed, std::hash<idx>{}(vertex.getTime()));

      return seed;
    }
  };

}


#endif /* TIMED_VERTEX_HH */
