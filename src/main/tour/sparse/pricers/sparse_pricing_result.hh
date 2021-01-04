#ifndef SPARSE_PRICING_RESULT_HH
#define SPARSE_PRICING_RESULT_HH

#include "timed/time_expanded_graph.hh"
#include "timed/timed_path.hh"

class SparsePricingResult
{
private:
  std::vector<TimedEdge> edges;
  std::vector<TimedPath> paths;
  std::optional<double> lowerBound;

public:
  SparsePricingResult()
    : lowerBound(-inf)
  {}

  SparsePricingResult(std::vector<TimedPath> paths,
                      std::optional<double> lowerBound = {})
    : paths(paths),
      lowerBound(lowerBound)
  {}

  SparsePricingResult(std::vector<TimedEdge> edges,
                      std::optional<double> lowerBound = {})
    : edges(edges),
      lowerBound(lowerBound)
  {}

  const std::vector<TimedEdge>& getEdges() const
  {
    return edges;
  }

  const std::vector<TimedPath>& getPaths() const
  {
    return paths;
  }

  bool isEmpty() const
  {
    return paths.empty() && edges.empty();
  }

  static SparsePricingResult empty()
  {
    return SparsePricingResult();
  }

  std::optional<double> getLowerBound() const
  {
    return lowerBound;
  }

};


#endif /* SPARSE_PRICING_RESULT_HH */
