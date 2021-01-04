#ifndef EXPAND_TOUR_HH
#define EXPAND_TOUR_HH

#include "tour/tour.hh"

#include "timed/time_expanded_graph.hh"
#include "timed/timed_vertex_func.hh"

TimeExpandedGraph createTimeExpandedGraph(const Tour& tour,
                                          TimedDistanceFunc& distances,
                                          idx lowerBound = 0);

#endif /* EXPAND_TOUR_HH */
