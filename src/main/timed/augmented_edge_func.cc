#include "augmented_edge_func.hh"

void AugmentedEdgeFunc::generateCosts(const Edge& edge,
                                      num cost,
                                      idx timeHorizon,
                                      num scaleFactor,
                                      const ValueVector& times)
{
  timedCosts(edge).reserve(timeHorizon);

  num currentCost = 0;
  num factor = 1;
  num currentTime = 0;

  for(const num& nextTime : times)
  {
    if(factor == 1)
    {
      while(currentTime < nextTime)
      {
        timedCosts(edge).push_back(cost + std::max(std::min(currentCost++,
                                                            (scaleFactor - 1)*cost), 0));

        ++currentTime;
      }
    }
    else
    {
      while(currentTime < nextTime)
      {
        timedCosts(edge).push_back(cost + std::max(std::min(currentCost--,
                                                            (scaleFactor - 1)*cost), 0));

        ++currentTime;
      }
    }

    factor *= -1;
  }

  ;

}

void AugmentedEdgeFunc::generateCosts(const EdgeFunc<num>& costs,
                                      idx timeHorizon,
                                      num scaleFactor,
                                      const EdgeFunc<ValueVector>& times)
{
  for(const Edge& edge : graph.getEdges())
  {
    generateCosts(edge, costs(edge), timeHorizon, scaleFactor, times(edge));
  }

}
