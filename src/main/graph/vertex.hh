#ifndef VERTEX_HH
#define VERTEX_HH

#include <functional>
#include <iostream>

#include "util.hh"

/**
 * A vertex of a graph.
 */
class Vertex
{
private:
  idx index;
public:
  /**
   * Constructs a new vertex.
   *
   * @param index the index of the vertex
   */
  explicit Vertex(idx index) : index(index) {}

  Vertex();

  bool operator==(const Vertex& other) const;
  bool operator!=(const Vertex& other) const;

  /**
   * A comparison operator, based on indices.
   */
  bool operator<(const Vertex& other) const;
  bool operator<=(const Vertex& other) const;

  /**
   * Returns the index of this vertex.
   */
  idx getIndex() const;
};

std::ostream& operator<<(std::ostream& out, const Vertex& vertex);

namespace std
{
  /**
   * A hash function for vertices. Returns the indices of vertices.
   */
  template<> struct hash<Vertex>
  {
    typedef Vertex argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& vertex) const
    {
      return vertex.getIndex();
    }
  };
}


#endif /* VERTEX_HH */
