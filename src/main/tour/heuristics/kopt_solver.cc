#include "kopt_solver.hh"

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <sstream>

#include "log.hh"

namespace
{
  typedef std::vector<Vertex>::const_iterator VertexIterator;
  typedef std::vector<VertexIterator> MultiIterator;
  typedef std::pair<VertexIterator, VertexIterator> Slice;

  struct SliceComparator
  {
    bool operator()(const Slice& first, const Slice& second) const
    {
      return first.first < second.first;
    }
  };

  std::vector<Slice> createSlices(const MultiIterator& multiIterator)
  {
    std::vector<Slice> slices;

    if(multiIterator.size() == 0)
    {
      return {};
    }

    auto last = multiIterator.begin();
    auto curr = last;
    ++curr;

    for(; curr != multiIterator.end(); ++curr)
    {
      slices.push_back(std::make_pair(*last, *curr));
      last = curr;
    }

    assert(slices.size() + 1 == multiIterator.size());

    return slices;
  }

  MultiIterator createMultiIterator(std::vector<Vertex>& vertices,
                                    num k,
                                    KOptSolver::DifferenceType initialDistance)
  {
    assert(k + initialDistance <= ((int) vertices.size()));

    VertexIterator it = vertices.begin();

    std::advance(it, initialDistance);

    MultiIterator multiIterator;

    for(num i = 0; i < k; ++i)
    {
      multiIterator.push_back(it);
      ++it;
    }

    return multiIterator;
  }

  void insertSlices(const std::vector<Slice>& slices,
                    std::uint64_t inverter,
                    std::vector<Vertex>& vertices)
  {
    idx i = 0;
    for(const Slice& slice : slices)
    {
      if(inverter & (1 << i))
      {
        vertices.insert(vertices.end(),
                        std::reverse_iterator<VertexIterator>(slice.second),
                        std::reverse_iterator<VertexIterator>(slice.first));
      }
      else
      {
        vertices.insert(vertices.end(), slice.first, slice.second);
      }
    }
  }

  bool advanceMultiIterator(MultiIterator& multiIterator,
                            const VertexIterator end)
  {
    idx i = 1;

    for(auto backward = multiIterator.rbegin(); backward != multiIterator.rend(); ++backward, ++i)
    {
      auto dist = std::distance(*backward, end);

      // can't increase any more...
      if(dist <= i)
      {
        continue;
      }
      else
      {
        ++(*backward);
        VertexIterator curr = *backward;
        for(auto forward = backward.base(); forward != multiIterator.end(); ++forward)
        {
          if(++curr == end)
          {
            return false;
          }
          *forward = curr;
        }

        return true;
      }
    }

    return false;
  }

  /*
  void printMultiIterator(const MultiIterator& multiIterator,
                          const std::vector<Vertex>& vertices)
  {
    std::ostringstream namebuf;

    const VertexIterator begin = vertices.begin();

    namebuf << "[";

    for(const VertexIterator& it : multiIterator)
    {
      namebuf << std::distance(begin, it) << ", ";
    }

    namebuf << "]";

    //Log(debug) << namebuf.str() << std::endl;
  }
  */
}

KOptSolver::KOptSolver(const Graph& graph,
                       const std::vector<Vertex>& vertices,
                       const idx k)
  : graph(graph),
    vertices(vertices),
    k(k)
{
  if(vertices.size() < k or k > 64)
  {
    throw std::invalid_argument("Invalid choice of parameter");
  }
}


Tour KOptSolver::findTour(TourEvaluator<>& evaluator,
                          const Tour& initialTour)
{
  if(not(initialTour.connects(vertices)))
  {
    throw std::invalid_argument("Given tour does not connect vertices");
  }


  Tour currentTour(graph, initialTour.getVertices());

  DifferenceType difference = 1;

  do
  {
    difference = improve(evaluator, currentTour, difference);
  }
  while(difference != (DifferenceType) -1);

  assert(currentTour.connects(vertices));

  Vertex source = *(vertices.begin());

  currentTour.swapToBegin(source);

  return currentTour;
}

KOptSolver::DifferenceType
KOptSolver::improve(TourEvaluator<>& evaluator,
                    Tour& tour,
                    DifferenceType initialDistance)
{
  std::vector<Vertex>& tourVertices = tour.getVertices();

  const num initialCost = evaluator(tour);

  if(tourVertices.size() < k)
  {
    throw std::invalid_argument("Tour has too few vertices");
  }

  MultiIterator multiIterator = createMultiIterator(tourVertices,
                                                    k,
                                                    initialDistance);

  Tour nextTour(graph, tourVertices);
  std::vector<Vertex>& nextVertices = nextTour.getVertices();
  nextVertices.reserve(tourVertices.size());

  const std::uint64_t minInverter = 1;
  const std::uint64_t maxInverter = 1 << k;

  do {

    const VertexIterator first = *(multiIterator.begin());
    const VertexIterator last = *(multiIterator.rbegin());

    const VertexIterator begin = tourVertices.begin();
    const VertexIterator end = tourVertices.end();

    std::vector<Slice> slices = createSlices(multiIterator);

    do {
      for(std::uint64_t curr = minInverter; curr < maxInverter; ++curr)
      {
        nextVertices.resize(0);

        nextVertices.insert(nextVertices.end(), begin, first);

        insertSlices(slices, curr, nextVertices);

        nextVertices.insert(nextVertices.end(), last, end);

        assert(nextVertices.size() == tourVertices.size());

        const num nextCost = evaluator(nextTour);
        const num costDifference = nextCost - initialCost;

        assert(*nextVertices.begin() == *vertices.begin());

        if(costDifference < 0)
        {
          DifferenceType distance = std::distance(begin, *(multiIterator.begin()));
          Log(info) << "Cost decreased by " << -costDifference;
          tourVertices = nextVertices;

          return distance;
        }

      }
    } while(std::next_permutation(slices.begin(), slices.end(), SliceComparator{}));

    /*
    if(debuggingEnabled())
    {
      printMultiIterator(multiIterator, tourVertices);
    }
    */
  }
  while (advanceMultiIterator(multiIterator, tourVertices.end()));

  return (DifferenceType) -1;
}
