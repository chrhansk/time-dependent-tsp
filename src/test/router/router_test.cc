#include <algorithm>
#include <iostream>
#include <vector>

#include <gtest/gtest.h>

#include "graph/graph.hh"
#include "router/router.hh"
#include "router/bidirectional_router.hh"

class RouterTest : public testing::Test
{
protected:
  Graph* graph;
  EdgeMap<num>* costs;
  Vertex source, target;
  Edge shortcut;

public:
  RouterTest();
  ~RouterTest();

};

class NotEqual
{
private:
  const Edge& forbiddenEdge;
public:
  NotEqual(const Edge& forbiddenEdge)
    : forbiddenEdge(forbiddenEdge)
  {
  }
  bool operator()(const Edge& edge)
  {
    return !(edge == forbiddenEdge);
  }
};

RouterTest::RouterTest()
{
  std::vector<Vertex> vertices;
  std::vector<Edge> edges;

  for(int i = 0; i < 10; ++i)
  {
    vertices.push_back(Vertex(i));
  }

  for(unsigned int i = 0; i < vertices.size() - 1; ++i)
  {
    edges.push_back(Edge(vertices[i], vertices[i + 1], i));
  }

  graph = new Graph(vertices.size(), edges);

  source = *(vertices.begin());
  target = *(vertices.rbegin());

  shortcut = graph->addEdge(source, target);

  costs = new EdgeMap<num>(*graph, 1);
  (*costs)(shortcut) = 100;
}

RouterTest::~RouterTest()
{
  delete costs;
  delete graph;
}

TEST_F(RouterTest, testRouterSimple)
{
  SearchResult<> result = Dijkstra<>(*graph)
    .shortestPath(source, target, costs->getValues());

  ASSERT_TRUE(result.found);

  ASSERT_EQ(9, result.path.cost(costs->getValues()));
}

TEST_F(RouterTest, testRouterBounded)
{
  SearchResult<> result = Dijkstra<>(*graph)
    .shortestPath(source, target, costs->getValues(), 7);

  ASSERT_FALSE(result.found);
}

TEST_F(RouterTest, testBidirectional)
{
  SearchResult<> result = BidirectionalRouter<>(*graph)
    .shortestPath(source, target, costs->getValues());

  ASSERT_TRUE(result.found);

  ASSERT_EQ(9, result.path.cost(costs->getValues()));
}

TEST_F(RouterTest, testPredicate)
{
  (*costs)(shortcut) = 0;
  BidirectionalRouter<> router(*graph);

  SearchResult<> result = router.shortestPath(source,
                                               target,
                                               costs->getValues());

  ASSERT_TRUE(result.found);

  ASSERT_EQ(0, result.path.cost(costs->getValues()));

  NotEqual predicate(shortcut);

  result = router.shortestPath<NotEqual,
                               NotEqual,
                               false>(source,
                                      target,
                                      costs->getValues(),
                                      predicate,
                                      predicate,
                                      inf);

  ASSERT_TRUE(result.found);

  ASSERT_EQ(9, result.path.cost(costs->getValues()));
}

TEST_F(RouterTest, testPredicateBound)
{
  (*costs)(shortcut) = 0;
  BidirectionalRouter<> router(*graph);
  NotEqual predicate(shortcut);

  SearchResult<> result = router.shortestPath<NotEqual,
                                               NotEqual,
                                               false>(source,
                                                      target,
                                                      costs->getValues(),
                                                      predicate,
                                                      predicate,
                                                      inf);

  ASSERT_TRUE(result.found);

  ASSERT_EQ(9, result.path.cost(costs->getValues()));

  result = router.shortestPath<NotEqual,
                               NotEqual,
                               true>(source,
                                     target,
                                     costs->getValues(),
                                     predicate,
                                     predicate,
                                     5);

  ASSERT_FALSE(result.found);

  result = router.shortestPath<NotEqual,
                               NotEqual,
                               true>(source,
                                     target,
                                     costs->getValues(),
                                     predicate,
                                     predicate,
                                     9);

  ASSERT_TRUE(result.found);

  ASSERT_EQ(9, result.path.cost(costs->getValues()));
}
