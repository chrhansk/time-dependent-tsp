#ifndef CACHE_HH
#define CACHE_HH

#include <unordered_map>
#include <list>
#include <functional>

#include "util.hh"

template<class Key,
         class Value,
         class Hash = std::hash<Key>>
class Cache
{
public:
  typedef std::function<Value(const Key&)> Compute;
private:
  std::list<Key> entries;

  // Key to value and key history iterator
  std::unordered_map<Key,
                     std::pair<Value, typename std::list<Key>::iterator>,
                     Hash> cache;

  idx hits, misses, capacity;
  Compute func;

  void evict();

public:
  Cache(const Compute& func, idx capacity = 100000)
    : hits(0),
      misses(0),
      capacity(capacity),
      func(func)
  {}

  Value get(const Key& key);
  bool contains(const Key& key);
  void insert(const Key& key, const Value& value);
};

template<class Key,
         class Value,
         class Hash>
bool Cache<Key, Value, Hash>::contains(const Key& key)
{
  auto it = cache.find(key);

  return it != std::end(cache);
}

template<class Key,
         class Value,
         class Hash>
Value Cache<Key, Value, Hash>::get(const Key& key)
{
  auto it = cache.find(key);

  if(it == std::end(cache))
  {
    ++misses;

    Value value = func(key);

    insert(key, value);
    return value;
  }
  else
  {
    ++hits;
    entries.splice(std::end(entries), entries, (it)->second.second);

    // return the retrieved value
    return (it)->second.first;
  }
}

template<class Key,
         class Value,
         class Hash>
void Cache<Key, Value, Hash>::evict()
{
  assert(!entries.empty());

  // identify least-recently-used key
  auto it = cache.find(entries.front());

  //erase both elements to completely purge record
  cache.erase(it);
  entries.pop_front();
}

template<class Key,
         class Value,
         class Hash>
void Cache<Key, Value, Hash>::insert(const Key& key, const Value& value)
{
  if(cache.find(key) != cache.end())
  {
    return;
  }

  // make space if necessary
  if(entries.size() == capacity)
  {
    evict();
  }

  // record k as most-recently-used key
  typename std::list<Key>::iterator it = entries.insert(std::end(entries), key);

  // create key-value entry, linked to the usage record
  cache.insert(std::make_pair(key, std::make_pair(value, it)));
}


#endif /* CACHE_HH */
