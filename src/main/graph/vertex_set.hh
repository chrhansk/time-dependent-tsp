#ifndef VERTEX_SET_HH
#define VERTEX_SET_HH

#include <vector>

class Graph;
class Vertex;
class Edge;

/**
 * A set of vertices. Realized as a vector of
 * boolean values.
 **/
class VertexSet
{
private:
  std::vector<bool> values;
public:
  /**
   * Creates a new (empty) VertexSet
   **/
  VertexSet(const Graph& graph);

  /**
   * Returns whether the given Vertex is contained
   * in this VertexSet.
   **/
  bool contains(const Vertex& vertex) const;

  /**
   * Inserts a given Vertex into this VertexSet.
   **/
  void insert(const Vertex& vertex);

  /**
   * Removes a given Vertex from this VertexSet.
   **/
  void remove(const Vertex& vertex);

  bool contains(const Edge& edge);
};

#endif /* VERTEX_SET_HH */
