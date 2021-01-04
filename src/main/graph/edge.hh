#ifndef EDGE_HH
#define EDGE_HH

/** @file */

#include "vertex.hh"

class VertexSet;

/**
 * @enum Direction An enum containing possible directions
 **/
enum class Direction
{
  /** The outgoing direction **/
  OUTGOING,
  /** The incoming direction **/
  INCOMING
};

template <class T>
class Bidirected
{
private:
  T outgoing;
  T incoming;

public:
  template<typename... Args>
  Bidirected(Args&&... args)
    : outgoing(std::forward<Args>(args)...),
      incoming(std::forward<Args>(args)...)
  {

  }

  /*
  Bidirected(const T& incoming, const T& outgoing)
    : incoming(incoming), outgoing(outgoing)
  {}
  */

  template<Direction direction>
  const T& get() const
  {
    return direction == Direction::OUTGOING ?
      outgoing :
      incoming;
  }

  template<Direction direction>
  T& get()
  {
    return direction == Direction::OUTGOING ?
      outgoing :
      incoming;
  }

  T& get(Direction direction)
  {
    return direction == Direction::OUTGOING ?
      outgoing :
      incoming;
  }

  const T& get(Direction direction) const
  {
    return direction == Direction::OUTGOING ?
      outgoing :
      incoming;
  }
};

/**
 * Returns the opposite of the given direction.
 *
 * @param direction The given direction
 *
 * @return The opposite direction
 */
constexpr Direction opposite(Direction direction)
{
  return (direction == Direction::OUTGOING)
    ? Direction::INCOMING
    : Direction::OUTGOING;
}


/**
 * An edge, defined by its source and target and a unique index.
 **/
class Edge
{
private:
  Vertex source, target;
  idx index;

public:

  /**
   * Constructs a new edge.
   *
   * @param source The source vertex
   * @param target The target vertex
   * @param index  The unique index
   */
  explicit Edge(Vertex source, Vertex target, idx index)
    : source(source),
      target(target),
      index(index)
  {}

  Edge()
  {}

  Edge& operator=(const Edge& other) = default;

  Edge(const Edge& other) = default;

  /**
   * Returns the source of this edge.
   */
  Vertex getSource() const;

  /**
   * Returns the target of this edge.
   */
  Vertex getTarget() const;

  /**
   * Returns the endpoint of this edge with respect to the given direction
   *
   * @param direction The given direction
   *
   * @return The endpoint
   */
  Vertex getEndpoint(Direction direction) const;

  /**
   * Returns the endpoint which is opposite to the given endpoint
   *
   * @param vertex A given endpoint
   *
   * @return The opposite endpoint
   */
  Vertex getOpposite(Vertex vertex) const;

  /**
   * Returns the index if this edge.
   */
  idx getIndex() const;

  /**
   * A comparison operator, based on indices.
   */
  bool operator==(const Edge& other) const;

  bool operator!=(const Edge& other) const;

  bool enters(const VertexSet& vertices) const;

  bool leaves(const VertexSet& vertices) const;

  bool intersects(const VertexSet& vertices) const;

};

namespace std
{
  /**
   * A hash function for edges
   */
  template<> struct hash<Edge>
  {
    typedef Edge argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& edge) const
    {
      result_type seed = 0;
      compute_hash_combination(seed, edge.getSource());
      compute_hash_combination(seed, edge.getTarget());
      return seed;
    }
  };
}

#endif /* EDGE_HH */
