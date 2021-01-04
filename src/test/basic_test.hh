#include <vector>

#include <gtest/gtest.h>

#include "graph/graph.hh"
#include "graph/edge_map.hh"
#include "graph/vertex_map.hh"

#include "router/router.hh"

class AbstractTest : public testing::Test
{
protected:
  Graph graph;
  EdgeMap<num> costMap;
  VertexMap<Point> points;

public:
  AbstractTest();
};

class BasicTest : public AbstractTest
{
public:
  BasicTest();

protected:
  EdgeValueMap<num> costs;
  std::vector<Vertex> sources, targets;
};

class BasicRouterTest : public BasicTest
{
private:
  class Result
  {
  public:
    Result(Vertex source, Vertex target, num cost)
      : source(source),
        target(target),
        cost(cost)
    {}
    Vertex source, target;
    num cost;
  };
  std::vector<Result> results;
public:
  BasicRouterTest();

  void testRouter(Router<>& router) const;
};
