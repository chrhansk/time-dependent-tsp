#ifndef EDGE_SET_HH
#define EDGE_SET_HH

#include <vector>

class Graph;
class Edge;

/**
 * A set of edges. Realized as a vector of
 * boolean values.
 **/
class EdgeSet
{
private:
  std::vector<bool> values;
public:
  /**
   * Creates a new (empty) EdgeSet
   **/
  EdgeSet(const Graph& graph);

  /**
   * Returns whether the given Edge is contained
   * in this EdgeSet.
   **/
  bool contains(const Edge& vertex) const;

  /**
   * Inserts a given Edge into this EdgeSet.
   **/
  void insert(const Edge& vertex);

  /**
   * Removes a given Edge from this EdgeSet.
   **/
  void remove(const Edge& vertex);
};

#endif /* EDGE_SET_HH */
