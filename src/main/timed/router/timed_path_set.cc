#include "timed_path_set.hh"

std::optional<double> TimedPathSet::cutoffValue() const
{
  if(valuedPaths.empty())
  {
    return {};
  }

  return valuedPaths.back().value;
}

std::optional<double> TimedPathSet::bestValue() const
{
  if(valuedPaths.empty())
  {
    return {};
  }

  return valuedPaths.front().value;
}

bool TimedPathSet::insert(const TimedPath& timedPath,
                          double value)
{
  auto pair = allPaths.insert(timedPath);

  const bool inserted = pair.second;

  if(!inserted)
  {
    return false;
  }

  if(value >= cutoffValue().value_or(inf))
  {
    return false;
  }

  valuedPaths.push_back(ValuedPath{value, timedPath});

  std::push_heap(std::begin(valuedPaths),
                 std::end(valuedPaths),
                 [&](const ValuedPath& first, const ValuedPath& second) -> bool
                 {
                   return first.value > second.value;
                 });

  if(maxSize && valuedPaths.size() > *maxSize)
  {
    valuedPaths.resize(*maxSize, {});
  }

  return true;
}
