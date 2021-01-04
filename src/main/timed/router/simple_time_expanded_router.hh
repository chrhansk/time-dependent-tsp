#ifndef SIMPLE_TIME_EXPANDED_ROUTER_HH
#define SIMPLE_TIME_EXPANDED_ROUTER_HH

#include "time_expanded_router.hh"

class SimpleTimeExpandedRouter : public TimeExpandedRouter
{
public:
  SimpleTimeExpandedRouter(const TimeExpandedGraph& graph,
                           Vertex originalSource)
    : TimeExpandedRouter(graph, originalSource)
  {

  }

  Result findShortestPaths(const Request& request) override;
};


#endif /* SIMPLE_TIME_EXPANDED_ROUTER_HH */
