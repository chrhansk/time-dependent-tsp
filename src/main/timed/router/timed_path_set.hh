#ifndef TIMED_PATH_SET_HH
#define TIMED_PATH_SET_HH

#include <unordered_set>
#include <optional>
#include <vector>

#include "timed/timed_path.hh"

class TimedPathSet
{
private:
  std::optional<idx> maxSize;

  std::unordered_set<TimedPath> allPaths;

  struct ValuedPath
  {
    double value;
    TimedPath timedPath;
  };

  std::vector<ValuedPath> valuedPaths;

  typedef std::vector<ValuedPath>::const_iterator Iter;

public:

  class Iterator
  {
  private:
    Iter iter;

  public:
    Iterator(const Iter& iter)
      : iter(iter)
    {}

    bool operator!=(const Iterator& other)
    {
      return iter != other.iter;
    }

    const TimedPath& operator*() const
    {
      return iter->timedPath;
    }

    const Iterator& operator++()
    {
      ++iter;
      return *this;
    }

  };

  struct IteratorPair
  {
    Iterator begin_iter;
    Iterator end_iter;

    Iterator begin() const
    {
      return begin_iter;
    }

    Iterator end() const
    {
      return end_iter;
    }
  };

public:

  TimedPathSet(const std::optional<idx> maxSize)
    : maxSize(maxSize)
  {}

  std::optional<double> cutoffValue() const;

  std::optional<double> bestValue() const;

  bool insert(const TimedPath& timedPath,
              double value);

  IteratorPair getPaths() const
  {
    return IteratorPair{Iterator(std::begin(valuedPaths)),
        Iterator(std::end(valuedPaths))};
  }

};


#endif /* TIMED_PATH_SET_HH */
