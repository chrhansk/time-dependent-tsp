#ifndef PATH_BASED_PRICING_RESULT_HH
#define PATH_BASED_PRICING_RESULT_HH

#include <optional>

#include "timed/time_expanded_graph.hh"
#include "timed/timed_path.hh"

class PathBasedPricingResult
{
private:
  std::vector<TimedPath> paths;
  std::optional<double> lowerBound;

public:
  PathBasedPricingResult()
    : lowerBound(-inf)
  {}

  PathBasedPricingResult(std::vector<TimedPath> paths,
                         std::optional<double> lowerBound = {})
    : paths(paths),
      lowerBound(lowerBound)
  {}

  const std::vector<TimedPath>& getPaths() const
  {
    return paths;
  }

  bool isEmpty() const
  {
    return paths.empty();
  }

  static PathBasedPricingResult empty()
  {
    return PathBasedPricingResult();
  }

  std::optional<double> getLowerBound() const
  {
    return lowerBound;
  }

};



#endif /* PATH_BASED_PRICING_RESULT_HH */
