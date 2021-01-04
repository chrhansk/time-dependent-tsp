#include "dk_separator.hh"

#include <sstream>

#include "log.hh"

const double cutoff = 1e-3;

namespace
{
  class Entry
  {
  private:
    num index, predecessor;
    Vertex vertex;
    double weight, bound;

  public:
    Entry(const Vertex& vertex,
          num index)
      : index(index),
        predecessor(-1),
        vertex(vertex),
        weight(0),
        bound(0)
    {
      assert(index >= 0);
    }

    Entry(const Vertex& vertex,
          num index,
          num predecessor,
          double weight,
          double bound)
      : index(index),
        predecessor(predecessor),
        vertex(vertex),
        weight(weight),
        bound(bound)
    {
      assert(index >= 0);
      assert(predecessor >= 0);
      assert(weight >= 0);
    }

    num getIndex() const
    {
      return index;
    }

    num getPredecessor() const
    {
      return predecessor;
    }

    const Vertex& getVertex() const
    {
      return vertex;
    }

    double getWeight() const
    {
      return weight;
    }

    double getBound() const
    {
      return bound;
    }

  };

}

double DKSeparator::getViolation(const EdgeFunc<double>& weights,
                                 const std::vector<Vertex>& vertices)
{
  const num k = vertices.size();

  assert(k >= 2);

  double violation = -k + 1;

  EdgeMap<num> factors = computeFactors(vertices,
                                        computeIndices(vertices).getValues());

  for(const Edge& edge : graph.getEdges())
  {
    violation += factors(edge) * weights(edge);
  }

  return violation;
}

VertexMap<num>
DKSeparator::computeIndices(const std::vector<Vertex>& vertices)
{
  VertexMap<num> indices(graph, -1);

  {
    idx currentIndex = 0;
    for(const Vertex& vertex : vertices)
    {
      indices(vertex) = currentIndex++;
    }
  }

  return indices;
}

EdgeMap<num>
DKSeparator::computeFactors(const std::vector<Vertex>& vertices,
                            const VertexFunc<num>& indices)
{
  const num k = vertices.size();

  EdgeMap<num> factors(graph, 0);

  const num firstIndex = 0;
  const num lastIndex = k - 1;

  for(const Edge& edge : graph.getEdges())
  {
    const num sourceIndex = indices(edge.getSource());
    const num targetIndex = indices(edge.getTarget());

    if(sourceIndex == -1 || targetIndex == -1)
    {
      continue;
    }

    if(sourceIndex == firstIndex)
    {
      if(targetIndex == lastIndex)
      {
        factors(edge) = 1;
      }
      else
      {
        factors(edge) = 2;
      }
    }
    else
    {
      if(targetIndex + 1 == sourceIndex)
      {
        factors(edge) = 1;
      }
      else
      {
        if(sourceIndex < targetIndex)
        {
          if(sourceIndex > firstIndex &&
             targetIndex >= 2 &&
             targetIndex < lastIndex)
          {
            factors(edge) = 1;
          }
        }
      }
    }
  }

  return factors;
}

std::vector<std::vector<Vertex>>
DKSeparator::separate(const EdgeFunc<double>& weights,
                      int maxNumCycles)
{
  std::vector<std::vector<Vertex>> cycles;
  int numCycles = 0;

  std::vector<Entry> entries;
  std::queue<Entry> unprocessed;

  auto addEntry = [&] (const Entry& entry)
    {
      entries.push_back(entry);
      unprocessed.push(entry);
    };

  auto weight = [&] (const Vertex& source, const Vertex& target)
    -> double
    {
      for(const Edge& edge : graph.getOutgoing(source))
      {
        if(edge.getTarget() == target)
        {
          return weights(edge);
        }
      }

      throw std::invalid_argument("Could not find edge");
    };

  auto getVertices = [&] (const Entry& entry)
    -> std::vector<Vertex>
    {
      std::vector<Vertex> vertices;

      assert(entry.getIndex() >= 0);

      vertices.push_back(entry.getVertex());

      Entry currentEntry = entry;

      while(currentEntry.getPredecessor() >= 0)
      {
        currentEntry = entries.at(currentEntry.getPredecessor());
        vertices.push_back(currentEntry.getVertex());
      }

      std::reverse(std::begin(vertices), std::end(vertices));

      return vertices;
    };

  auto hasValidWeight = [&] (const Entry& entry)
    -> bool
    {
      auto vertices = getVertices(entry);

      if(vertices.size() <= 1)
      {
        return entry.getWeight() == 0;
      }

      double violation = getViolation(weights, vertices);

      return fabs(violation - entry.getWeight()) <= 1e-4;
    };

  for(const Vertex& vertex : graph.getVertices())
  {
    addEntry(Entry(vertex, entries.size()));
  }

  while(!unprocessed.empty())
  {
    Entry currentEntry = unprocessed.front();
    unprocessed.pop();

    assert(hasValidWeight(currentEntry));

    auto vertices = getVertices(currentEntry);
    VertexSet vertexSet(graph);

    for(const Vertex& vertex : vertices)
    {
      vertexSet.insert(vertex);
    }

    Vertex firstVertex = vertices.front();
    Vertex lastVertex = vertices.back();

    for(const Vertex& nextVertex : graph.getVertices())
    {
      if(vertexSet.contains(nextVertex))
      {
        continue;
      }

      if(currentEntry.getBound() + weight(lastVertex, nextVertex) < 0)
      {
        continue;
      }

      double nextWeight = currentEntry.getWeight();
      nextWeight += weight(firstVertex, nextVertex);
      nextWeight += weight(nextVertex, lastVertex);

      for(const Vertex& currentVertex : vertices)
      {
        if(currentVertex != lastVertex)
        {
          nextWeight += weight(currentVertex, lastVertex);
        }
      }

      nextWeight -= 1;

      double nextBound = currentEntry.getBound();
      nextBound += weight(nextVertex, lastVertex);

      for(const Vertex& currentVertex : vertices)
      {
        nextBound += weight(currentVertex, nextVertex);
      }

      nextBound -= 1;

      if(nextWeight < 0)
      {
        continue;
      }

      Entry nextEntry(nextVertex,
                      entries.size(),
                      currentEntry.getIndex(),
                      nextWeight,
                      nextBound);

      assert(getVertices(nextEntry).size() ==
             getVertices(currentEntry).size() + 1);

      if(getVertices(nextEntry).size() == graph.getVertices().size())
      {
        continue;
      }

      assert(hasValidWeight(nextEntry));

      addEntry(nextEntry);

      if(nextWeight >= cutoff)
      {
        auto nextVertices = vertices;
        nextVertices.push_back(nextVertex);

        bool hasDominatingPrefix = false;

        {
          Entry entry = currentEntry;

          while(entry.getPredecessor() >= 0)
          {
            if(entry.getWeight() >= nextEntry.getWeight())
            {
              hasDominatingPrefix = true;
              break;
            }

            entry = entries.at(entry.getPredecessor());
          }

        }

        /*
         * We disregard dominated cuts.
         */
        if(hasDominatingPrefix)
        {
          continue;
        }

        /*
         * Cuts are symmetric on cycles of size two.
         * We break symmetry to decrease the total number
         * of cuts.
         */
        if(nextVertices.size() == 2)
        {
          if(nextVertices.back() < nextVertices.front())
          {
            continue;
          }
        }

        if(debuggingEnabled())
        {
          auto actualVertices = getVertices(nextEntry);

          assert(nextVertices == actualVertices);

          std::ostringstream namebuf;

          namebuf << "[";

          for(const Vertex& vertex : nextVertices)
          {
            namebuf << vertex << ", ";
          }

          namebuf << "]";

          Log(debug) << "Adding cycle: "
                     << namebuf.str()
                     << ", weight: "
                     << nextWeight;

        }

        cycles.push_back(nextVertices);
        ++numCycles;

        if(maxNumCycles != -1 && numCycles >= maxNumCycles)
        {
          return cycles;
        }

      }
    }

  }

  return cycles;
}
