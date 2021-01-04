#ifndef TUPLES_HH
#define TUPLES_HH

template<class T, class Collection = std::vector<T>>
class TupleIterator
{
private:
  const Collection& collection;
  typedef typename Collection::const_iterator Iterator;
  std::vector<Iterator> iterators;
  std::vector<T> items;

  bool finished;

  void reset(idx current)
  {
    Iterator it = std::begin(collection);
    std::advance(it, current);
    set(current, it);
  }

  void set(idx current, Iterator& iterator)
  {
    iterators[current] = iterator;
    items[current] = *iterator;
  }

public:
  TupleIterator(const Collection& collection, idx size)
    : collection(collection),
      finished(false)
  {
    assert(!collection.empty());

    iterators.resize(size, std::begin(collection));
    items.resize(size, *std::begin(collection));
    for(idx current = 0; current < size; ++current)
    {
      reset(current);
    }
  }

  TupleIterator(const Collection& collection)
    : collection(collection),
      finished(true)
  {

  }

  bool operator !=(const TupleIterator& other) const
  {
    return finished != other.finished;
  }

  TupleIterator operator++()
  {
    idx i = 1;
    const idx size = iterators.size();

    Iterator end = std::end(collection);

    for(auto backward = iterators.rbegin(); backward != iterators.rend(); ++backward, ++i)
    {
      Iterator current = *backward;
      auto dist = std::distance(current, end);

      // can't increase any more...
      if(dist <= i)
      {
        continue;
      }
      else
      {
        for(idx j = 0; j < i ;++j)
        {
          set(size - i + j, ++current);
        }

        return *this;
      }
    }

    finished = true;
    return *this;
  }

  const std::vector<T>& operator*() { return items; }

  static TupleIterator end(const Collection& collection)
  {
    return TupleIterator(collection);
  }

  static TupleIterator begin(const Collection& collection, idx size)
  {
    return TupleIterator(collection, size);
  }

};

template<class T, class Collection = std::vector<T>>
class Tuples
{
private:
  const Collection& collection;
  idx size;

public:
  Tuples(const Collection& collection, idx size)
    : collection(collection),
      size(size)
  {}

  TupleIterator<T, Collection> begin()
  {
    return TupleIterator<T, Collection>::begin(collection, size);
  }

  TupleIterator<T, Collection> end()
  {
    return TupleIterator<T, Collection>::end(collection);
  }
};

template<class T>
Tuples<T, std::vector<T>> tuples(const std::vector<T>& items, idx size)
{
  return Tuples<T>(items, size);
}


#endif /* TUPLES_HH */
