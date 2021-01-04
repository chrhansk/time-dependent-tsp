#ifndef UTIL_HH
#define UTIL_HH

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <memory>
#include <random>
#include <vector>
#include <unordered_map>

#include "cmp.hh"
#include "defs.hh"

/** @file **/

typedef uint32_t idx;
typedef int32_t num;
typedef unsigned int uint;

typedef std::vector<num> ValueVector;

extern num inf;

/**
 * A class modeling a Point consisting of
 * two coordinates.
 **/
class Point
{
private:
  double x, y;

public:
  Point(double x, double y)
    : x(x), y(y) {}

  double getX() const
  {
    return x;
  }

  double getY() const
  {
    return y;
  }

  bool operator==(const Point& other) const
  {
    return getX() == other.getX() && getY() == other.getY();
  }

  double distance(const Point& other) const
  {
    return std::hypot(getX() - other.getX(),
                      getY() - other.getY());
  }
};

template <class T>
inline void compute_hash_combination(std::size_t & seed, const T & v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{

  template <>
  struct hash<Point>
  {
    typedef std::size_t result_type;

    result_type operator()(const Point& p) const
    {
      result_type seed = 0;
      compute_hash_combination(seed, std::hash<double>{}(p.getX()));
      compute_hash_combination(seed, std::hash<double>{}(p.getY()));

      return seed;
    }
  };
}

template <class Cont, class T>
bool contains(const Cont& cont, const T& value)
{
  return std::find(std::begin(cont), std::end(cont), value) != std::end(cont);
}


struct PairHash {
  typedef std::size_t result_type;

  template <class T1, class T2>
  std::size_t operator () (const std::pair<T1, T2> &p) const {
    result_type seed = 0;
    compute_hash_combination(seed, std::hash<T1>{}(p.first));
    compute_hash_combination(seed, std::hash<T2>{}(p.second));

    return seed;
  }
};

template <class T, class R>
class PairMap
{
public:
  typedef std::pair<T, T> Pair;
private:
  std::unordered_map<Pair, R, PairHash> map;

public:
  const R& operator()(const T& first, const T& second) const
  {
    return map.at(std::make_pair(first, second));
  }

  R& operator()(const T& first, const T& second)
  {
    return map.at(std::make_pair(first, second));
  }

  void put(const T&first, const T& second, const R&value)
  {
    map[std::make_pair(first, second)] = value;
  }
};

/**
 * Shuffles a range based on a given seed.
 **/
template <class RAIter>
void shuffle(RAIter begin, RAIter end, int seed = 42)
{
  auto engine = std::default_random_engine(seed);

  std::shuffle(begin, end, engine);
}

/**
 * Returns whether the debug mode is enabled.
 **/
constexpr bool debuggingEnabled()
{
#if defined(NDEBUG)
  return false;
#else
  return true;
#endif
}

#endif /* UTIL_HH */
