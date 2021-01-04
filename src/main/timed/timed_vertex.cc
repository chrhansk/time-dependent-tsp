#include "timed_vertex.hh"


idx TimedVertex::getIndex() const {
  return index;
}

idx TimedVertex::getTime() const {
  return time;
}

bool TimedVertex::operator==(const TimedVertex& other) const
{
  return getIndex() == other.getIndex() &&
    getTime() == other.getTime();
}

bool TimedVertex::operator!=(const TimedVertex& other) const
{
  return !(*this == other);
}


bool TimedVertex::operator<(const TimedVertex& other) const
{
  if(getIndex() == other.getIndex())
  {
    return getTime() < other.getTime();
  }

  return getIndex() < other.getIndex();
}
