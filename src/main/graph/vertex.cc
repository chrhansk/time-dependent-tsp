#include "vertex.hh"

Vertex::Vertex()
  : index(std::numeric_limits<idx>::max())
{
}

idx Vertex::getIndex() const
{
  return index;
}

bool Vertex::operator==(const Vertex& other) const
{
  return getIndex() == other.getIndex();
}

bool Vertex::operator!=(const Vertex& other) const
{
  return !(*this == other);
}

bool Vertex::operator<(const Vertex& other) const
{
  return getIndex() < other.getIndex();
}

bool Vertex::operator<=(const Vertex& other) const
{
  return getIndex() <= other.getIndex();
}

std::ostream& operator<<(std::ostream& out, const Vertex& vertex)
{
  out << vertex.getIndex();
  return out;
}
