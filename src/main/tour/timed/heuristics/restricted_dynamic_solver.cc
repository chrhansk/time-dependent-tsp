#include "restricted_dynamic_solver.hh"

#include "log.hh"

#include <algorithm>
#include <bitset>
#include <set>

namespace
{
  const idx setSize = 64;
  typedef std::bitset<setSize> BitSet;

  struct Entry
  {
    BitSet entries;
    idx lastEntry;
    num arrivalTime;
    const Entry* predecessor;

    Entry(const Entry& other,
          idx lastEntry,
          num arrivalTime)
      : entries(other.entries),
        lastEntry(lastEntry),
        arrivalTime(arrivalTime),
        predecessor(&other)
    {
      assert(!other.entries[lastEntry]);
      entries[lastEntry] = 1;
    }

    Entry(idx entry)
      : lastEntry(entry),
        arrivalTime(0),
        predecessor(nullptr)
    {
      entries[lastEntry] = 1;
    }

    std::vector<Vertex> getVertices(const std::vector<Vertex>& vertices) const
    {
      const Entry* currentEntry = this;
      std::vector<Vertex> currentVertices;

      while(currentEntry != nullptr)
      {
        currentVertices.push_back(vertices[currentEntry->lastEntry]);
        currentEntry = currentEntry->predecessor;
      }

      std::reverse(currentVertices.begin(), currentVertices.end());

      return currentVertices;
    }
  };

  struct CostCompare
  {
    bool operator()(const Entry& first, const Entry& second) const
    {
      return first.arrivalTime < second.arrivalTime;
    }
  };

  struct BitSetCompare
  {
    bool operator()(const BitSet& first, const BitSet& second) const
    {
      for(idx i = 0; i < setSize; ++i)
      {
        if(not(first[i]) and second[i])
        {
          return true;
        }

        if(first[i] != second[i])
        {
          return false;
        }
      }

      return false;
    }
  };

  struct SetCompare
  {
    bool operator()(const Entry& first, const Entry& second) const
    {
      if(first.entries == second.entries)
      {
        return first.arrivalTime < second.arrivalTime;
      }

      return BitSetCompare{}(first.entries, second.entries);
    }
  };
}

RestrictedDynamicSolver::RestrictedDynamicSolver(const Graph& graph,
                                                 const std::vector<Vertex>& vertices,
                                                 idx maxSize)
  : graph(graph),
    vertices(vertices),
    maxSize(maxSize)
{}

Tour RestrictedDynamicSolver::findTour(TimedDistanceFunc& timedDistances)
{
  std::vector<Entry> initialEntries;

  /*
    // if the source of the tour is fixed:
    initialEntries.push_back(Entry(0));
   */
  for(idx i = 0; i < vertices.size(); ++i)
  {
    initialEntries.push_back(Entry(i));
  }

  std::vector<std::vector<Entry>> allEntries{std::move(initialEntries)};

  for(idx i = 0; i < vertices.size() - 1; ++i)
  {
    Log(info) << "Starting iteration " << (i + 1) << " of " << vertices.size();

    const std::vector<Entry> &lastEntries = *(allEntries.rbegin());

    Log(info) << "Continuing " << lastEntries.size() << " tours";

    assert(lastEntries.begin()->entries.count() < vertices.size());

    num upperBound = inf;

    std::vector<Entry> nextEntries;
    nextEntries.reserve(maxSize);

    for(const Entry& lastEntry : lastEntries)
    {
      std::set<Entry, CostCompare> bestCandidates;

      for(idx next = 0; next < vertices.size(); ++next)
      {
        if(lastEntry.entries[next])
        {
          continue;
        }

        num arrivalTime = lastEntry.arrivalTime + timedDistances(vertices[lastEntry.lastEntry],
                                                                 vertices[next],
                                                                 lastEntry.arrivalTime);

        if(arrivalTime <= upperBound)
        {
          Entry nextEntry(lastEntry, next, arrivalTime);
          bestCandidates.insert(nextEntry);

          if(bestCandidates.size() >= maxSize)
          {
            auto it = bestCandidates.begin();
            std::advance(it, maxSize);
            upperBound = std::min(upperBound, it->arrivalTime);
          }
        }
      }

      nextEntries.insert(nextEntries.begin(), bestCandidates.begin(), bestCandidates.end());
    }

    assert(nextEntries.size() >= std::min(maxSize, (idx) lastEntries.size()));

    if(nextEntries.size() <= maxSize or i == 0)
    {
      allEntries.push_back(std::move(nextEntries));
      continue;
    }

    std::sort(nextEntries.begin(), nextEntries.end(), SetCompare{});

    auto last = nextEntries.begin();
    auto it = last;
    ++it;

    std::vector<Entry> bestEntries;

    bestEntries.push_back(*last);

    for(;it != nextEntries.end();++it)
    {
      if(last->entries != it->entries)
      {
        bestEntries.push_back(*it);
      }

      last = it;
    }

    if(bestEntries.size() > maxSize)
    {
      std::sort(bestEntries.begin(), bestEntries.end(), CostCompare{});
      bestEntries.resize(maxSize, Entry(0));
    }

    allEntries.push_back(std::move(bestEntries));
  }

  const std::vector<Entry> &lastEntries = *(allEntries.rbegin());
  num bestTime = inf;
  Entry bestEntry(0);

  for(const Entry& entry : lastEntries)
  {
    assert(entry.entries.count() == vertices.size());

    std::vector<Vertex> entryVertices = entry.getVertices(vertices);

    num arrivalTime = entry.arrivalTime + timedDistances(*(entryVertices.rbegin()),
                                                         *(entryVertices.begin()),
                                                         entry.arrivalTime);

    if (arrivalTime < bestTime)
    {
      bestTime = arrivalTime;
      bestEntry = entry;
    }
  }

  std::vector<Vertex> bestVertices = bestEntry.getVertices(vertices);

  assert(bestVertices.size() == vertices.size());

  Log(info) << "Best arrival time: " << bestTime;

  Tour bestTour(graph, bestVertices);

  Vertex source = *(vertices.begin());

  bestTour.swapToBegin(source);

  return bestTour;
}
