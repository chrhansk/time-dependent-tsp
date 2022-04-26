#ifndef TIMED_READER_HH
#define TIMED_READER_HH

#include <fstream>

#include "graph/vertex.hh"
#include "graph/vertex_map.hh"
#include "timed_edge_map.hh"

class TimedReadResult
{
public:
  Graph graph;
  VertexMap<std::string> vertexNames;
  EdgeMap<std::string> edgeNames;

  TimedEdgeMap<num> travelTimes;
};

TimedReadResult readTimedInstance(std::ifstream& input);

#endif /* TIMED_READER_HH */
