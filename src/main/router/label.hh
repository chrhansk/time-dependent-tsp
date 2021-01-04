#ifndef LABEL_HH
#define LABEL_HH

#include "graph/graph.hh"

enum class State
{
  UNKNOWN, LABELED, SETTLED
};

template<class T = num>
class AbstractLabel
{
private:
  Vertex vertex;
  T cost;
  State state;

public:
  AbstractLabel()
    : cost(inf),
      state(State::UNKNOWN)
  {}
  AbstractLabel(Vertex vertex, T cost)
    : vertex(vertex),
      cost(cost),
      state(State::LABELED)
  {}

  T getCost() const
  {
    return cost;
  }

  bool operator<(const AbstractLabel& other) const
  {
    return cost < other.cost;
  }

  bool operator>(const AbstractLabel& other) const
  {
    return cost > other.cost;
  }

  Vertex getVertex() const
  {
    return vertex;
  }

  State getState() const
  {
    return state;
  }

  void setState(State state)
  {
    this->state = state;
  }

};

template<class T = num>
class SimpleLabel : public AbstractLabel<T>
{
public:
  SimpleLabel(Vertex vertex, T cost)
    : AbstractLabel<T>(vertex, cost) {}
  SimpleLabel() {}
};

template<class T = num>
class Label : public AbstractLabel<T>
{
private:
  Edge edge;

public:
  Label(Vertex vertex, const Edge& edge, T cost)
    : AbstractLabel<T>(vertex, cost),
      edge(edge)
  {}

  Label() {}

  Edge getEdge() const
  {
    return edge;
  }
};

#endif /* LABEL_HH */
