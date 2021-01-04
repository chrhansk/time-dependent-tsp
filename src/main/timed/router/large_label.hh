#ifndef LARGE_LABEL_HH
#define LARGE_LABEL_HH

#include <array>
#include <memory>

#include "time_expanded_router.hh"

template<idx size> using LabelPrefix = std::array<Vertex, size>;

template<idx size>
struct LabelPrefixHasher
{
  std::size_t operator()(const LabelPrefix<size>& prefix) const
  {
    std::size_t seed = 0;

    std::hash<Vertex> vertexHash{};

    for(const auto& vertex : prefix)
    {
      compute_hash_combination(seed, vertexHash(vertex));
    }

    return seed;
  }
};



template<idx size>
class LargeLabel;

template<idx size>
using LabelPtr = std::shared_ptr<LargeLabel<size>>;

template<idx size>
class LargeLabel
{
private:
  LabelPrefix<size> prefix;
  double cost;
  TimedEdge timedEdge;
  LabelPtr<size> predecessor;

public:

  LargeLabel()
    : cost(inf)
  {}

  LargeLabel(const TimedVertex& timedVertex,
             const Vertex& vertex,
             double cost)
    : cost(cost),
      timedEdge(timedVertex, timedVertex, -1),
      predecessor(nullptr)
  {
    *prefix.rbegin() = vertex;
  }

  LargeLabel(const TimedEdge& timedEdge,
             const Vertex& vertex,
             double cost,
             LabelPtr<size> predecessor)
    : cost(cost),
      timedEdge(timedEdge),
      predecessor(predecessor)
  {
    {
      auto it = predecessor->getPrefix().begin();
      auto end = predecessor->getPrefix().end();
      ++it;

      assert(std::find(it, end, vertex) == end);
      assert(predecessor->getVertex() == timedEdge.getSource());

      assert(std::distance(it, end) + 1 == size);

      std::copy(it, end, prefix.begin());

      *(prefix.rbegin()) = vertex;
    }
  }

  bool operator==(const LargeLabel& other)
  {
    return getPrefix() == other.getPrefix();
  }

  LargeLabel& operator=(const LargeLabel& other)
  {
    assert(getPrefix() == other.getPrefix());

    cost = other.getCost();
    timedEdge = other.getEdge();
    predecessor = other.getPredecessor();

    return *this;
  }

  double getCost() const
  {
    return cost;
  }

  TimedVertex getVertex() const
  {
    return timedEdge.getTarget();
  }

  const TimedEdge& getEdge() const
  {
    return timedEdge;
  }

  bool canExtend(const Vertex& vertex) const
  {
    return !contains(prefix, vertex);
  }

  const LabelPrefix<size>& getPrefix() const
  {
    return prefix;
  }

  const LabelPtr<size> getPredecessor() const
  {
    return predecessor;
  }

  TimedPath createPath() const
  {
    TimedPath timedPath;

    if(!getPredecessor())
    {
      return timedPath;
    }

    timedPath.prepend(getEdge());

    LabelPtr<size> currentLabel = getPredecessor();

    while(currentLabel->getPredecessor())
    {
      timedPath.prepend(currentLabel->getEdge());
      currentLabel = currentLabel->getPredecessor();
    }

    return timedPath;
  }

};

template <idx size>
class LabelSet
{
public:
  typedef std::unordered_map<LabelPrefix<size>, LabelPtr<size>, LabelPrefixHasher<size>> Map;

  typedef typename Map::const_iterator MapIterator;

  struct Iterator
  {
    MapIterator iter;

    LabelPtr<size> operator*() const
    {
      return iter->second;
    }

    const Iterator& operator++()
    {
      ++iter;
      return *this;
    }

    const bool operator!=(const Iterator& other) const
    {
      return iter != other.iter;
    }
  };

private:
  Map labels;

public:

  Iterator begin() const
  {
    return Iterator{labels.begin()};
  }

  Iterator end() const
  {
    return Iterator{labels.end()};
  }

  bool insert(LabelPtr<size> label)
  {
    auto pair = labels.insert({label->getPrefix(), label});

    // was inserted...
    if(pair.second)
    {
      return true;
    }

    auto iterator = pair.first;

    LabelPtr<size> oldLabel = iterator->second;

    if(oldLabel->getCost() <= label->getCost())
    {
      return false;
    }

    // assign
    *oldLabel = *label;

    return false;
  }

};

template<idx size>
struct LabelCostComparator
{
  bool operator()(const LabelPtr<size>& first, const LabelPtr<size>& second) const
  {
    return first->getCost() < second->getCost();
  }
};

template<idx size>
using LargeLabels = std::vector<LargeLabel<size>>;


#endif /* LARGE_LABEL_HH */
