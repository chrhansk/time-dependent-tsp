#ifndef TIMER_HH
#define TIMER_HH

#include <algorithm>
#include <numeric>
#include <chrono>

#include "log.hh"

class Timer
{
private:
  std::chrono::time_point<std::chrono::steady_clock> start;

public:
  Timer()
  {
    reset();
  }

  void reset()
  {
    start = std::chrono::steady_clock::now();
  }

  double elapsed()
  {
    auto finish = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
  }
};

template<class Func>
class FunctionTimer
{
private:
  Func func;

public:
  FunctionTimer(const Func& func)
    : func(func)
  {}

  double execute()
  {
    Timer timer;

    func();

    return timer.elapsed();
  }
};

#endif /* TIMER_HH */
