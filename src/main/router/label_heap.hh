#ifndef LABEL_HEAP_HH
#define LABEL_HEAP_HH

#include <boost/heap/d_ary_heap.hpp>

#include "graph/graph.hh"
#include "graph/vertex_map.hh"

template <class T>
using SimpleHeap = boost::heap::d_ary_heap<T,
                                           boost::heap::mutable_<true>,
                                           boost::heap::compare<std::greater<T>>,
                                           boost::heap::arity<2>>;

template <class Label>
class LabelHeap
{
public:
  typedef SimpleHeap<Label> Heap;
private:
  typedef typename Heap::handle_type Handle;

  const Graph& graph;
  VertexMap<Label> labels;
  VertexMap<Handle> handles;
  Heap heap;
public:
  LabelHeap(const Graph& graph);

  const Label& getLabel(Vertex vertex) const;
  Label& getLabel(Vertex vertex);
  void update(Label label);
  const Label& extractMin();

  const Label& peek();

  bool finished() const;

  bool isEmpty() const;
};

template <class Label>
LabelHeap<Label>::LabelHeap(const Graph& graph)
  : graph(graph),
    labels(graph, Label()),
    handles(graph, Handle())
{
}

template <class Label>
const Label& LabelHeap<Label>::getLabel(Vertex vertex) const
{
  return labels(vertex);
}

template <class Label>
Label& LabelHeap<Label>::getLabel(Vertex vertex)
{
  return labels(vertex);
}

template <class Label>
void LabelHeap<Label>::update(Label label)
{
  Label& current = getLabel(label.getVertex());

  switch(current.getState())
  {
  case State::UNKNOWN:
    current = label;
    current.setState(State::LABELED);
    handles(current.getVertex()) = heap.push(current);
    break;
  case State::SETTLED:
    return;
  case State::LABELED:
    if(current > label)
    {
      Handle handle = handles(current.getVertex());
      heap.update(handle, label);
      current = label;
    }
    return;
  }

}

template <class Label>
const Label& LabelHeap<Label>::extractMin()
{
  assert(!isEmpty());
  const Label minLabel = heap.top();
  heap.pop();
  Label& label = getLabel(minLabel.getVertex());

  label.setState(State::SETTLED);

  return label;
}

template <class Label>
const Label& LabelHeap<Label>::peek()
{
  assert(!isEmpty());
  const Label& minLabel = heap.top();
  Label& label = getLabel(minLabel.getVertex());
  return label;
}

template <class Label>
bool LabelHeap<Label>::finished() const
{
  for(const Vertex& vertex : graph.getVertices())
  {
    if(getLabel(vertex).getState() == State::LABELED)
    {
      return false;
    }
  }

  return true;
}

template <class Label>
bool LabelHeap<Label>::isEmpty() const
{
  return heap.empty();
}

#endif /* LABEL_HEAP_HH */
