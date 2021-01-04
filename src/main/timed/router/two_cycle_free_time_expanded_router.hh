#ifndef TWO_CYCLE_FREE_TIME_EXPANDED_ROUTER_HH
#define TWO_CYCLE_FREE_TIME_EXPANDED_ROUTER_HH

#include "time_expanded_router.hh"

class TwoCycleFreeTimeExpandedRouter : public TimeExpandedRouter
{
public:
  TwoCycleFreeTimeExpandedRouter(const TimeExpandedGraph& graph,
                                 Vertex originalSource)
    : TimeExpandedRouter(graph, originalSource)
  {

  }

  Result findShortestPaths(const Request& request) override;
};


#endif /* TWO_CYCLE_FREE_TIME_EXPANDED_ROUTER_HH */
